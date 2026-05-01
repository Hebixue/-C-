import math
from collections import Counter, defaultdict
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


HALF_CORE_CM = 60.0
GRID_MIN_CM = -180
GRID_MAX_CM = 180
RSSI_NOISE_RAW = 70.0

ANTENNAS = {
    1: ("LEFT", (-HALF_CORE_CM, 0.0)),
    2: ("RIGHT", (HALF_CORE_CM, 0.0)),
    3: ("FRONT", (0.0, HALF_CORE_CM)),
    4: ("REAR", (0.0, -HALF_CORE_CM)),
}

ZONE_NAMES = {
    0x00: "MASK_OR_UNKNOWN",
    0x01: "LEFT",
    0x02: "RIGHT",
    0x03: "REAR",
    0x04: "FRONT",
    0x05: "CORE",
}

# Current firmware table, converted from pkes_ranging.c x2 values to raw RSSI.
# Distances are in cm.
RSSI_TABLE = [
    (9.0, 2726.5),
    (14.0, 2562.0),
    (19.0, 2253.5),
    (24.0, 2209.0),
    (29.0, 2171.5),
    (34.0, 1850.0),
    (39.0, 1736.0),
    (44.0, 1640.0),
    (49.0, 1563.0),
    (54.0, 1506.5),
    (59.0, 1454.5),
    (64.0, 1174.5),
    (69.0, 1109.5),
    (74.0, 1059.0),
    (79.0, 1021.5),
    (84.0, 977.5),
    (89.0, 942.0),
    (94.0, 909.0),
    (99.0, 869.0),
    (104.0, 851.0),
    (109.0, 815.0),
    (114.0, 786.0),
    (119.0, 763.5),
    (124.0, 746.0),
    (129.0, 729.0),
    (139.0, 685.0),
    (149.0, 655.0),
    (159.0, 624.5),
    (179.0, 616.0),
]


def true_zone(x, y):
    if -HALF_CORE_CM <= x <= HALF_CORE_CM and -HALF_CORE_CM <= y <= HALF_CORE_CM:
        return 0x05
    if x < -HALF_CORE_CM and -HALF_CORE_CM <= y <= HALF_CORE_CM:
        return 0x01
    if x > HALF_CORE_CM and -HALF_CORE_CM <= y <= HALF_CORE_CM:
        return 0x02
    if -HALF_CORE_CM <= x <= HALF_CORE_CM and y < -HALF_CORE_CM:
        return 0x03
    if -HALF_CORE_CM <= x <= HALF_CORE_CM and y > HALF_CORE_CM:
        return 0x04
    return 0x00


def distance_to(point, ant_id):
    ax, ay = ANTENNAS[ant_id][1]
    return math.hypot(point[0] - ax, point[1] - ay)


def rssi_from_distance(distance_cm):
    if distance_cm <= RSSI_TABLE[0][0]:
        return RSSI_TABLE[0][1]
    if distance_cm >= RSSI_TABLE[-1][0]:
        return RSSI_TABLE[-1][1]

    for i in range(len(RSSI_TABLE) - 1):
        d0, r0 = RSSI_TABLE[i]
        d1, r1 = RSSI_TABLE[i + 1]
        if d0 <= distance_cm <= d1:
            ratio = (distance_cm - d0) / (d1 - d0)
            return r0 + ratio * (r1 - r0)

    raise RuntimeError("RSSI interpolation failed")


def rssi_features(rssi):
    # Positive x_feature means Ant2/right RSSI is stronger than Ant1/left.
    # Positive front_feature means Ant3/front RSSI is stronger than Ant4/rear.
    x_feature = rssi[2] - rssi[1]
    front_feature = rssi[3] - rssi[4]
    return x_feature, front_feature


def build_grid_arrays():
    points = []
    zones = []
    margins = []
    rssi_rows = []
    for x in range(GRID_MIN_CM, GRID_MAX_CM + 1):
        for y in range(GRID_MIN_CM, GRID_MAX_CM + 1):
            point = (x, y)
            rssi = [0.0] * 4
            for ant_id in ANTENNAS:
                rssi[ant_id - 1] = rssi_from_distance(distance_to(point, ant_id))
            points.append(point)
            zones.append(true_zone(x, y))
            margins.append(min(abs(abs(x) - HALF_CORE_CM), abs(abs(y) - HALF_CORE_CM)))
            rssi_rows.append(rssi)

    return {
        "points": np.array(points, dtype=float),
        "zone": np.array(zones, dtype=np.int16),
        "margin": np.array(margins, dtype=float),
        "rssi": np.array(rssi_rows, dtype=float),
    }


def classify_rssi_vector(x_feature, front_feature, center_th, side_th):
    x_state = np.full_like(x_feature, 9, dtype=np.int8)
    y_state = np.full_like(front_feature, 9, dtype=np.int8)

    x_state[np.abs(x_feature) <= center_th] = 0
    x_state[x_feature > side_th] = 1
    x_state[x_feature < -side_th] = -1

    y_state[np.abs(front_feature) <= center_th] = 0
    y_state[front_feature > side_th] = 1
    y_state[front_feature < -side_th] = -1

    pred = np.full_like(x_state, -1, dtype=np.int16)
    known = (x_state != 9) & (y_state != 9)
    pred[known & (x_state == 0) & (y_state == 0)] = 0x05
    pred[known & (x_state == -1) & (y_state == 0)] = 0x01
    pred[known & (x_state == 1) & (y_state == 0)] = 0x02
    pred[known & (x_state == 0) & (y_state == -1)] = 0x03
    pred[known & (x_state == 0) & (y_state == 1)] = 0x04
    pred[known & (x_state != 0) & (y_state != 0)] = 0x00
    return pred


def pct(values):
    return np.percentile(values, [0, 1, 5, 50, 95, 99, 100])


def print_feature_summary(arrays):
    rssi = arrays["rssi"]
    zone = arrays["zone"]
    x_feature = rssi[:, 1] - rssi[:, 0]
    front_feature = rssi[:, 2] - rssi[:, 3]

    print("Per-zone 1cm RSSI feature summary")
    print("x_feature = Ant2_RSSI - Ant1_RSSI, positive means RIGHT")
    print("front_feature = Ant3_RSSI - Ant4_RSSI, positive means FRONT")
    print()

    for zone_id in [0x01, 0x02, 0x03, 0x04, 0x05, 0x00]:
        mask = zone == zone_id
        xf = x_feature[mask]
        yf = front_feature[mask]
        print(f"{ZONE_NAMES[zone_id]} count={int(np.count_nonzero(mask))}")
        print(f"  x_feature     min/p1/p5/p50/p95/p99/max = {pct(xf).round(1).tolist()}")
        print(f"  front_feature min/p1/p5/p50/p95/p99/max = {pct(yf).round(1).tolist()}")
        print()


def evaluate_thresholds(arrays, center_th, side_th, noise_raw, trials, seed, margin_filter=None):
    rng = np.random.default_rng(seed)
    mask = np.ones(arrays["zone"].shape, dtype=bool)
    if margin_filter is not None:
        mask &= arrays["margin"] >= margin_filter

    base_rssi = arrays["rssi"][mask]
    zone = arrays["zone"][mask]
    correct = 0
    decided = 0
    total = 0
    false_valid_from_mask = 0
    mask_total = 0

    for _ in range(trials):
        noisy = base_rssi + rng.uniform(-noise_raw, noise_raw, size=base_rssi.shape)
        x_feature = noisy[:, 1] - noisy[:, 0]
        front_feature = noisy[:, 2] - noisy[:, 3]
        pred = classify_rssi_vector(x_feature, front_feature, center_th, side_th)
        known = pred >= 0
        correct += int(np.count_nonzero(pred[known] == zone[known]))
        decided += int(np.count_nonzero(known))
        total += zone.size

        mask_zone = zone == 0x00
        mask_total += int(np.count_nonzero(mask_zone))
        false_valid_from_mask += int(np.count_nonzero(mask_zone & ((pred == 0x01) | (pred == 0x02))))

    decided_accuracy = correct / decided if decided else 0.0
    coverage = decided / total if total else 0.0
    mask_false_valid_rate = false_valid_from_mask / mask_total if mask_total else 0.0
    return decided_accuracy, coverage, mask_false_valid_rate


def threshold_sweep(arrays):
    print(f"Threshold sweep with +/-{RSSI_NOISE_RAW:.0f} raw RSSI independent noise")
    print("Rule: |axis|<=CENTER => center, |axis|>=SIDE => side, between => UNKNOWN/hold previous")
    print("Important risk metric: diagonal MASK misclassified as LEFT/RIGHT valid.")
    print()

    for margin in [None, 5.0, 10.0]:
        label = "all points" if margin is None else f"excluding <= {margin:.0f}cm from grid boundaries"
        print(label)
        candidates = []
        for center_th in range(60, 181, 20):
            for side_th in range(center_th + 40, 421, 20):
                acc, coverage, false_mask = evaluate_thresholds(
                    arrays,
                    center_th,
                    side_th,
                    RSSI_NOISE_RAW,
                    trials=8,
                    seed=5000 + center_th * 10 + side_th,
                    margin_filter=margin,
                )
                if false_mask <= 0.02:
                    score = acc * min(coverage / 0.75, 1.0) * (1.0 - false_mask * 5.0)
                    candidates.append((score, acc, coverage, false_mask, center_th, side_th))

        for _, acc, coverage, false_mask, center_th, side_th in sorted(candidates, reverse=True)[:10]:
            print(
                f"  CENTER={center_th:3d} SIDE={side_th:3d} "
                f"decided_acc={acc * 100:6.2f}% coverage={coverage * 100:6.2f}% "
                f"mask_to_LR={false_mask * 100:5.2f}%"
            )
        print()


def plot_feature_scatter(arrays, output_path):
    rssi = arrays["rssi"]
    zone = arrays["zone"]
    points = arrays["points"]
    x_feature = rssi[:, 1] - rssi[:, 0]
    front_feature = rssi[:, 2] - rssi[:, 3]
    colors = {
        0x01: "#1f77b4",
        0x02: "#ff7f0e",
        0x03: "#2ca02c",
        0x04: "#9467bd",
        0x05: "#8c564b",
        0x00: "#7f7f7f",
    }

    fig, ax = plt.subplots(figsize=(9, 8))
    plot_mask = ((points[:, 0] % 4) == 0) & ((points[:, 1] % 4) == 0)
    for zone_id in [0x05, 0x01, 0x02, 0x03, 0x04, 0x00]:
        mask = plot_mask & (zone == zone_id)
        ax.scatter(
            x_feature[mask],
            front_feature[mask],
            s=4,
            alpha=0.35,
            color=colors[zone_id],
            label=ZONE_NAMES[zone_id],
        )

    for value in [-220, -120, 120, 220]:
        ax.axvline(value, color="#333333", linewidth=0.7, linestyle="--")
        ax.axhline(value, color="#333333", linewidth=0.7, linestyle="--")
    ax.axvline(0, color="#666666", linewidth=0.7)
    ax.axhline(0, color="#666666", linewidth=0.7)
    ax.set_xlabel("Ant2_RSSI - Ant1_RSSI (positive RIGHT)")
    ax.set_ylabel("Ant3_RSSI - Ant4_RSSI (positive FRONT)")
    ax.set_title("RSSI Features on 1cm 3x3 Grid, plotted every 4cm")
    ax.grid(True, linewidth=0.4, alpha=0.35)
    ax.legend(markerscale=3)
    fig.tight_layout()
    fig.savefig(output_path, dpi=180)
    plt.close(fig)


def main():
    arrays = build_grid_arrays()
    print_feature_summary(arrays)
    threshold_sweep(arrays)
    output_path = Path(__file__).resolve().parent / "pkes_rssi_1cm_grid_feature_scatter.png"
    plot_feature_scatter(arrays, output_path)
    print(f"Saved RSSI feature scatter: {output_path}")


if __name__ == "__main__":
    main()
