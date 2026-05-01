import itertools
from dataclasses import dataclass

import numpy as np
from scipy.spatial import cKDTree

import analyze_pkes_rssi_decision as base


NOISE_RAW = 70.0
TRIALS = 5
RANDOM_SEED = 20260501
EVAL_STEP_CM = 3

TEMPLATE_STEP_CM = [3, 5]
K_NEIGHBORS = [1, 3, 5, 9]
SCORE_MODES = ["nearest", "mean"]
RATIO_MARGINS = [0.00, 0.05, 0.10, 0.15, 0.20]

# Ant1=left, Ant2=right, Ant3=front, Ant4=rear. 0-based in code.
# These modes still classify all 5 outside classes; they only change which
# antenna dimensions are used for template distance after a handle trigger.
SUBSET_MODES = {
    "all4": {
        "left": (0, 1, 2, 3),
        "right": (0, 1, 2, 3),
    },
    "handle_side3": {
        # User-proposed: touched side antenna + front + rear.
        "left": (0, 2, 3),
        "right": (1, 2, 3),
    },
    "handle_lr_frontrear": {
        # Keep left/right axis, drop the opposite longitudinal antenna.
        "left": (0, 1, 2),
        "right": (0, 1, 3),
    },
    "handle_drop_opposite_side": {
        # Drop the opposite side antenna.
        "left": (0, 2, 3),
        "right": (1, 2, 3),
    },
    "side_axis_only": {
        "left": (0, 1),
        "right": (0, 1),
    },
}

ZONE_ORDER = [0x00, 0x01, 0x02, 0x03, 0x04]


@dataclass(frozen=True)
class Strategy:
    subset_mode: str
    template_step_cm: int
    k: int
    score_mode: str
    ratio_margin: float


def point_step_mask(points, step_cm):
    return ((points[:, 0] % step_cm) == 0) & ((points[:, 1] % step_cm) == 0)


def build_data():
    arrays = base.build_grid_arrays()
    eval_mask = arrays["zone"] != 0x05
    eval_mask &= point_step_mask(arrays["points"], EVAL_STEP_CM)
    return arrays, arrays["rssi"][eval_mask], arrays["zone"][eval_mask]


def build_zone_trees(arrays, step_cm, subset):
    template_mask = arrays["zone"] != 0x05
    template_mask &= point_step_mask(arrays["points"], step_cm)
    template_rssi = arrays["rssi"][template_mask][:, subset]
    template_zone = arrays["zone"][template_mask]

    trees = {}
    for zone in ZONE_ORDER:
        trees[zone] = cKDTree(template_rssi[template_zone == zone])
    return trees, int(template_zone.size)


def score_from_distances(distances, k, mode):
    if k == 1:
        return distances
    if mode == "nearest":
        return distances[:, 0]
    if mode == "mean":
        return np.mean(distances, axis=1)
    raise ValueError(mode)


def compute_scores(observed_rssi, trees, subset, k, mode):
    observed = observed_rssi[:, subset]
    scores = []
    for zone in ZONE_ORDER:
        distances, _ = trees[zone].query(observed, k=k, p=1, workers=-1)
        scores.append(score_from_distances(np.asarray(distances), k, mode))
    return np.vstack(scores).T


def classify(scores, ratio_margin):
    zone_values = np.array(ZONE_ORDER, dtype=np.int16)
    best_index = np.argmin(scores, axis=1)
    pred = zone_values[best_index].copy()

    best_score = scores[np.arange(scores.shape[0]), best_index]
    mask_score = scores[:, 0]
    valid_pred = np.isin(pred, [0x01, 0x02, 0x03, 0x04])
    improvement = (mask_score - best_score) / np.maximum(mask_score, 1e-6)
    pred[valid_pred & (improvement < ratio_margin)] = 0x00
    return pred


def confusion_matrix(true_zone, pred_zone):
    confusion = np.zeros((5, 5), dtype=np.int64)
    np.add.at(confusion, (true_zone.astype(int), pred_zone.astype(int)), 1)
    return confusion


def summarize(confusion):
    valid_rows = [1, 2, 3, 4]
    lr_rows = [1, 2]
    valid_cols = [1, 2, 3, 4]
    lr_cols = [1, 2]
    total = confusion.sum()
    valid_total = confusion[valid_rows, :].sum()
    lr_total = confusion[lr_rows, :].sum()
    mask_total = confusion[0, :].sum()
    valid_correct = sum(confusion[i, i] for i in valid_rows)
    lr_correct = confusion[1, 1] + confusion[2, 2]
    return {
        "overall_acc": np.trace(confusion) / total,
        "valid_recall": valid_correct / valid_total,
        "lr_recall": lr_correct / lr_total,
        "valid_to_mask": confusion[valid_rows, 0].sum() / valid_total,
        "mask_to_valid": confusion[0, valid_cols].sum() / mask_total,
        "mask_to_lr": confusion[0, lr_cols].sum() / mask_total,
        "confusion": confusion,
    }


def pct(value):
    return f"{value * 100:6.2f}%"


def print_result(strategy, left_stats, right_stats, both_stats, templates):
    left_subset = "".join(str(i + 1) for i in SUBSET_MODES[strategy.subset_mode]["left"])
    right_subset = "".join(str(i + 1) for i in SUBSET_MODES[strategy.subset_mode]["right"])
    print(
        f"  mode={strategy.subset_mode:22s} "
        f"Lants={left_subset:4s} Rants={right_subset:4s} "
        f"step={strategy.template_step_cm}cm "
        f"k={strategy.k:2d} score={strategy.score_mode:7s} "
        f"ratio={strategy.ratio_margin * 100:4.0f}% "
        f"templates={templates:5d} "
        f"L_valid={pct(left_stats['valid_recall'])} L_maskLR={pct(left_stats['mask_to_lr'])} "
        f"R_valid={pct(right_stats['valid_recall'])} R_maskLR={pct(right_stats['mask_to_lr'])} "
        f"both_valid={pct(both_stats['valid_recall'])} both_maskLR={pct(both_stats['mask_to_lr'])}"
    )


def main():
    arrays, eval_rssi, true_zone = build_data()
    rng = np.random.default_rng(RANDOM_SEED)
    noisy_trials = [
        eval_rssi + rng.uniform(-NOISE_RAW, NOISE_RAW, size=eval_rssi.shape)
        for _ in range(TRIALS)
    ]

    print("PKES handle-based dynamic-3 5-class search")
    print("Task: still classify MASK/LEFT/RIGHT/REAR/FRONT; subset depends on touched handle.")
    print(f"Noise model: independent uniform +/-{NOISE_RAW:.0f} raw RSSI on each antenna")
    print(f"Eval step: {EVAL_STEP_CM}cm, trials={TRIALS}")
    print()

    results = []
    tree_cache = {}
    configs = list(itertools.product(SUBSET_MODES.keys(), TEMPLATE_STEP_CM, K_NEIGHBORS, SCORE_MODES, RATIO_MARGINS))

    for idx, (subset_mode, step, k, mode, margin) in enumerate(configs, start=1):
        if k == 1 and mode == "mean":
            continue

        left_subset = SUBSET_MODES[subset_mode]["left"]
        right_subset = SUBSET_MODES[subset_mode]["right"]
        left_key = (step, left_subset)
        right_key = (step, right_subset)

        if left_key not in tree_cache:
            tree_cache[left_key] = build_zone_trees(arrays, step, left_subset)
        if right_key not in tree_cache:
            tree_cache[right_key] = build_zone_trees(arrays, step, right_subset)

        left_trees, template_count = tree_cache[left_key]
        right_trees, _ = tree_cache[right_key]
        left_conf = np.zeros((5, 5), dtype=np.int64)
        right_conf = np.zeros((5, 5), dtype=np.int64)

        for observed in noisy_trials:
            left_scores = compute_scores(observed, left_trees, left_subset, k, mode)
            right_scores = compute_scores(observed, right_trees, right_subset, k, mode)
            left_conf += confusion_matrix(true_zone, classify(left_scores, margin))
            right_conf += confusion_matrix(true_zone, classify(right_scores, margin))

        both_conf = left_conf + right_conf
        strategy = Strategy(subset_mode, step, k, mode, margin)
        results.append((strategy, summarize(left_conf), summarize(right_conf), summarize(both_conf), both_conf, template_count))

        if idx % 100 == 0:
            print(f"done {idx}/{len(configs)}")

    print()
    print("Candidates with both_valid >= 80%, sorted by both_maskLR")
    high = [item for item in results if item[3]["valid_recall"] >= 0.80]
    high.sort(key=lambda item: (item[3]["mask_to_lr"], -item[3]["valid_recall"]))
    if not high:
        print("  none")
    else:
        for strategy, left_stats, right_stats, both_stats, _, templates in high[:50]:
            print_result(strategy, left_stats, right_stats, both_stats, templates)

    print()
    print("Best under common both_maskLR risk limits")
    for limit in [0.005, 0.01, 0.02, 0.03, 0.05]:
        candidates = [item for item in results if item[3]["mask_to_lr"] <= limit]
        candidates.sort(key=lambda item: item[3]["valid_recall"], reverse=True)
        print()
        print(f"both_maskLR <= {limit * 100:.2f}%")
        if not candidates:
            print("  none")
            continue
        for strategy, left_stats, right_stats, both_stats, _, templates in candidates[:10]:
            print_result(strategy, left_stats, right_stats, both_stats, templates)

    print()
    print("Best tradeoff: both_valid - 8 * both_maskLR")
    tradeoff = sorted(results, key=lambda item: item[3]["valid_recall"] - 8.0 * item[3]["mask_to_lr"], reverse=True)
    for strategy, left_stats, right_stats, both_stats, _, templates in tradeoff[:25]:
        print_result(strategy, left_stats, right_stats, both_stats, templates)

    strategy, _, _, stats, confusion, _ = tradeoff[0]
    print()
    print("Confusion matrix for top tradeoff")
    print(strategy)
    print("rows=true, cols=pred, order=MASK LEFT RIGHT REAR FRONT, left+right combined")
    print(confusion)


if __name__ == "__main__":
    main()
