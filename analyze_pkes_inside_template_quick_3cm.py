import time
from dataclasses import dataclass

import numpy as np
from scipy.spatial import cKDTree

import analyze_pkes_rssi_decision as base


NOISE_RAW = 70.0
TRIALS = 5
RANDOM_SEED = 20260501
EVAL_STEP_CM = 1

CORE_TEMPLATE_STEP_CM = 3
OUTSIDE_TEMPLATE_STEP_CM = 5
K_NEIGHBORS = 9
SCORE_MODE = "mean"

RATIO_MARGINS = [0.00, 0.03, 0.05, 0.07, 0.10, 0.15, 0.20]
INSIDE_MAX_CM = [135, 140, 145, 150, 160]
AXIS_DIFF_MAX_CM = [50, 60, 70, 80, 100, 999]


@dataclass(frozen=True)
class Strategy:
    ratio_margin: float
    inside_max_cm: int
    axis_diff_max_cm: int


def point_step_mask(points, step_cm):
    return ((points[:, 0] % step_cm) == 0) & ((points[:, 1] % step_cm) == 0)


def build_eval_data(arrays):
    eval_mask = point_step_mask(arrays["points"], EVAL_STEP_CM)
    points = arrays["points"][eval_mask]
    rssi = arrays["rssi"][eval_mask]
    zone = arrays["zone"][eval_mask]
    distance = np.zeros((rssi.shape[0], 4), dtype=np.float64)

    for ant_id in base.ANTENNAS:
        ax, ay = base.ANTENNAS[ant_id][1]
        distance[:, ant_id - 1] = np.hypot(points[:, 0] - ax, points[:, 1] - ay)

    return rssi, zone, distance


def build_trees(arrays):
    core_mask = arrays["zone"] == 0x05
    core_mask &= point_step_mask(arrays["points"], CORE_TEMPLATE_STEP_CM)

    outside_mask = arrays["zone"] != 0x05
    outside_mask &= point_step_mask(arrays["points"], OUTSIDE_TEMPLATE_STEP_CM)

    return (
        cKDTree(arrays["rssi"][core_mask]),
        cKDTree(arrays["rssi"][outside_mask]),
        int(np.count_nonzero(core_mask)),
        int(np.count_nonzero(outside_mask)),
    )


def inside_prefilter(distance_cm, inside_max_cm, axis_diff_max_cm):
    all_near = np.all(distance_cm <= inside_max_cm, axis=1)
    x_balanced = np.abs(distance_cm[:, 0] - distance_cm[:, 1]) <= axis_diff_max_cm
    y_balanced = np.abs(distance_cm[:, 2] - distance_cm[:, 3]) <= axis_diff_max_cm
    return all_near & x_balanced & y_balanced


def evaluate(strategy, eval_rssi, true_zone, distance_cm, core_tree, outside_tree):
    rng = np.random.default_rng(RANDOM_SEED)
    confusion = np.zeros((2, 2), dtype=np.int64)
    true_inside = true_zone == 0x05
    allowed = inside_prefilter(distance_cm, strategy.inside_max_cm, strategy.axis_diff_max_cm)

    for _ in range(TRIALS):
        observed = eval_rssi + rng.uniform(-NOISE_RAW, NOISE_RAW, size=eval_rssi.shape)
        core_dist, _ = core_tree.query(observed, k=K_NEIGHBORS, p=1, workers=-1)
        outside_dist, _ = outside_tree.query(observed, k=K_NEIGHBORS, p=1, workers=-1)
        core_score = np.mean(core_dist, axis=1)
        outside_score = np.mean(outside_dist, axis=1)

        improvement = (outside_score - core_score) / np.maximum(outside_score, 1e-6)
        pred_inside = allowed & (core_score < outside_score) & (improvement >= strategy.ratio_margin)

        confusion[0, 0] += int(np.count_nonzero((~true_inside) & (~pred_inside)))
        confusion[0, 1] += int(np.count_nonzero((~true_inside) & pred_inside))
        confusion[1, 0] += int(np.count_nonzero(true_inside & (~pred_inside)))
        confusion[1, 1] += int(np.count_nonzero(true_inside & pred_inside))

    return confusion


def summarize(confusion):
    outside_total = confusion[0, :].sum()
    inside_total = confusion[1, :].sum()
    return {
        "false_inside": confusion[0, 1] / outside_total if outside_total else 0.0,
        "inside_recall": confusion[1, 1] / inside_total if inside_total else 0.0,
        "inside_miss": confusion[1, 0] / inside_total if inside_total else 0.0,
        "overall_acc": (confusion[0, 0] + confusion[1, 1]) / confusion.sum(),
    }


def pct(value):
    return f"{value * 100:6.2f}%"


def print_result(strategy, stats):
    axis = "none" if strategy.axis_diff_max_cm >= 999 else f"{strategy.axis_diff_max_cm}cm"
    print(
        f"  ratio={strategy.ratio_margin * 100:4.0f}% "
        f"inside_max={strategy.inside_max_cm:3d}cm "
        f"axis={axis:>5s} "
        f"inside_recall={pct(stats['inside_recall'])} "
        f"false_inside={pct(stats['false_inside'])} "
        f"inside_miss={pct(stats['inside_miss'])} "
        f"overall={pct(stats['overall_acc'])}"
    )


def main():
    start = time.time()
    arrays = base.build_grid_arrays()
    eval_rssi, true_zone, distance_cm = build_eval_data(arrays)
    core_tree, outside_tree, core_count, outside_count = build_trees(arrays)

    print("PKES inside/core quick search")
    print(f"CORE template step: {CORE_TEMPLATE_STEP_CM}cm, templates={core_count}")
    print(f"OUTSIDE reject step: {OUTSIDE_TEMPLATE_STEP_CM}cm, templates={outside_count}")
    print(f"k={K_NEIGHBORS}, mode={SCORE_MODE}, trials={TRIALS}")
    print(f"Noise model: independent uniform +/-{NOISE_RAW:.0f} raw RSSI on each antenna")
    print()

    results = []
    for ratio in RATIO_MARGINS:
        for inside_max in INSIDE_MAX_CM:
            for axis_diff in AXIS_DIFF_MAX_CM:
                strategy = Strategy(ratio, inside_max, axis_diff)
                confusion = evaluate(strategy, eval_rssi, true_zone, distance_cm, core_tree, outside_tree)
                results.append((strategy, summarize(confusion), confusion))

    print(f"done elapsed={time.time() - start:.1f}s")
    print()

    print("Candidates with inside_recall >= 95%, sorted by false_inside")
    high_recall = [item for item in results if item[1]["inside_recall"] >= 0.95]
    high_recall.sort(key=lambda item: (item[1]["false_inside"], -item[1]["inside_recall"]))
    if not high_recall:
        print("  none")
    else:
        for strategy, stats, _ in high_recall[:30]:
            print_result(strategy, stats)

    print()
    print("Best under false_inside risk limits")
    for limit in [0.001, 0.005, 0.01, 0.02, 0.05]:
        candidates = [item for item in results if item[1]["false_inside"] <= limit]
        candidates.sort(key=lambda item: item[1]["inside_recall"], reverse=True)
        print()
        print(f"false_inside <= {limit * 100:.2f}%")
        if not candidates:
            print("  none")
            continue
        for strategy, stats, _ in candidates[:10]:
            print_result(strategy, stats)

    print()
    print("Best tradeoff: inside_recall - 10 * false_inside")
    tradeoff = sorted(results, key=lambda item: item[1]["inside_recall"] - 10.0 * item[1]["false_inside"], reverse=True)
    for strategy, stats, _ in tradeoff[:20]:
        print_result(strategy, stats)

    strategy, stats, confusion = tradeoff[0]
    print()
    print("Confusion matrix for top tradeoff")
    print(strategy)
    print("rows=true [OUTSIDE INSIDE], cols=pred [OUTSIDE INSIDE]")
    print(confusion)


if __name__ == "__main__":
    main()
