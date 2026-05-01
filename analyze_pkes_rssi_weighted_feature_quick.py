import time
from dataclasses import dataclass

import numpy as np
from scipy.spatial import cKDTree

import analyze_pkes_rssi_decision as base


NOISE_RAW = 70.0
TRIALS = 5
RANDOM_SEED = 20260501
EVAL_STEP_CM = 1

TEMPLATE_STEPS_CM = [3, 5]
K_NEIGHBORS = 9
SCORE_MODE = "mean"
RATIO_MARGINS = [0.05, 0.10, 0.15, 0.20]

# name, sum_weight, axis_weight, rank_weight
FEATURE_PRESETS = [
    ("raw", 0.0, 0.0, 0.0),
    ("raw_axis", 0.0, 0.5, 0.0),
    ("raw_sum", 0.25, 0.0, 0.0),
    ("raw_axis_sum", 0.25, 0.5, 0.0),
    ("raw_axis_sum_rank", 0.25, 0.5, 0.10),
]

ZONE_ORDER = [0x00, 0x01, 0x02, 0x03, 0x04]


@dataclass(frozen=True)
class Strategy:
    name: str
    template_step_cm: int
    ratio_margin: float
    sum_weight: float
    axis_weight: float
    rank_weight: float


def point_step_mask(points, step_cm):
    return ((points[:, 0] % step_cm) == 0) & ((points[:, 1] % step_cm) == 0)


def rank_features(rssi):
    sorted_rssi = np.sort(rssi, axis=1)
    min_rssi = sorted_rssi[:, 0:1]
    second_max = sorted_rssi[:, 2:3]
    max_rssi = sorted_rssi[:, 3:4]
    spread = max_rssi - min_rssi
    return np.hstack((max_rssi, second_max, min_rssi, spread))


def make_features(rssi, sum_weight, axis_weight, rank_weight):
    rssi = rssi.astype(np.float64, copy=False)
    parts = [rssi]

    if sum_weight > 0.0:
        parts.append(np.sum(rssi, axis=1, keepdims=True) * sum_weight)

    if axis_weight > 0.0:
        x_axis = rssi[:, 1:2] - rssi[:, 0:1]
        y_axis = rssi[:, 2:3] - rssi[:, 3:4]
        parts.append(np.hstack((x_axis, y_axis)) * axis_weight)

    if rank_weight > 0.0:
        parts.append(rank_features(rssi) * rank_weight)

    return np.hstack(parts)


def build_data():
    arrays = base.build_grid_arrays()
    eval_mask = arrays["zone"] != 0x05
    eval_mask &= point_step_mask(arrays["points"], EVAL_STEP_CM)
    return arrays, arrays["rssi"][eval_mask], arrays["zone"][eval_mask]


def build_zone_trees(arrays, step_cm, sum_weight, axis_weight, rank_weight):
    template_mask = arrays["zone"] != 0x05
    template_mask &= point_step_mask(arrays["points"], step_cm)

    template_rssi = arrays["rssi"][template_mask]
    template_zone = arrays["zone"][template_mask]
    template_feature = make_features(template_rssi, sum_weight, axis_weight, rank_weight)

    trees = {}
    for zone in ZONE_ORDER:
        trees[zone] = cKDTree(template_feature[template_zone == zone])
    return trees, int(template_zone.size)


def compute_scores(observed_feature, trees):
    scores = []
    for zone in ZONE_ORDER:
        distances, _ = trees[zone].query(
            observed_feature,
            k=K_NEIGHBORS,
            p=1,
            workers=-1,
        )
        scores.append(np.mean(distances, axis=1))
    return np.vstack(scores).T


def predict(scores, ratio_margin):
    zone_values = np.array(ZONE_ORDER, dtype=np.int16)
    best_index = np.argmin(scores, axis=1)
    pred = zone_values[best_index].copy()

    best_score = scores[np.arange(scores.shape[0]), best_index]
    mask_score = scores[:, 0]
    improvement = (mask_score - best_score) / np.maximum(mask_score, 1e-6)
    valid_pred = np.isin(pred, [0x01, 0x02, 0x03, 0x04])
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


def print_result(strategy, stats, templates):
    print(
        f"  {strategy.name:18s} "
        f"step={strategy.template_step_cm}cm "
        f"ratio={strategy.ratio_margin * 100:4.0f}% "
        f"templates={templates:5d} "
        f"valid_recall={pct(stats['valid_recall'])} "
        f"lr_recall={pct(stats['lr_recall'])} "
        f"mask_to_lr={pct(stats['mask_to_lr'])} "
        f"mask_to_valid={pct(stats['mask_to_valid'])} "
        f"overall={pct(stats['overall_acc'])}"
    )


def main():
    start = time.time()
    arrays, eval_rssi, true_zone = build_data()
    rng = np.random.default_rng(RANDOM_SEED)
    noisy_trials = [
        eval_rssi + rng.uniform(-NOISE_RAW, NOISE_RAW, size=eval_rssi.shape)
        for _ in range(TRIALS)
    ]

    print("PKES quick weighted-feature search")
    print("Feature presets: raw, raw+axis, raw+sum, raw+axis+sum, raw+axis+sum+rank")
    print(f"Template steps: {TEMPLATE_STEPS_CM}, k={K_NEIGHBORS}, mode={SCORE_MODE}, trials={TRIALS}")
    print(f"Noise model: independent uniform +/-{NOISE_RAW:.0f} raw RSSI on each antenna")
    print()

    results = []
    for step in TEMPLATE_STEPS_CM:
        for name, sum_w, axis_w, rank_w in FEATURE_PRESETS:
            trees, template_count = build_zone_trees(arrays, step, sum_w, axis_w, rank_w)
            confusion_by_margin = {
                margin: np.zeros((5, 5), dtype=np.int64)
                for margin in RATIO_MARGINS
            }

            for observed_rssi in noisy_trials:
                observed_feature = make_features(observed_rssi, sum_w, axis_w, rank_w)
                scores = compute_scores(observed_feature, trees)
                for margin in RATIO_MARGINS:
                    confusion_by_margin[margin] += confusion_matrix(true_zone, predict(scores, margin))

            for margin, confusion in confusion_by_margin.items():
                strategy = Strategy(name, step, margin, sum_w, axis_w, rank_w)
                results.append((strategy, summarize(confusion), template_count))

            print(f"done step={step}cm preset={name} templates={template_count} elapsed={time.time() - start:.1f}s")

    print()
    print("All results sorted by mask_to_lr, then valid_recall")
    results_by_risk = sorted(results, key=lambda item: (item[1]["mask_to_lr"], -item[1]["valid_recall"]))
    for strategy, stats, templates in results_by_risk:
        print_result(strategy, stats, templates)

    print()
    print("Candidates with valid_recall >= 80%, sorted by mask_to_lr")
    high_valid = [item for item in results if item[1]["valid_recall"] >= 0.80]
    high_valid.sort(key=lambda item: (item[1]["mask_to_lr"], -item[1]["valid_recall"]))
    if not high_valid:
        print("  none")
    else:
        for strategy, stats, templates in high_valid:
            print_result(strategy, stats, templates)

    print()
    print("Best tradeoff: valid_recall - 8 * mask_to_lr")
    tradeoff = sorted(results, key=lambda item: item[1]["valid_recall"] - 8.0 * item[1]["mask_to_lr"], reverse=True)
    for strategy, stats, templates in tradeoff[:10]:
        print_result(strategy, stats, templates)

    strategy, stats, _ = tradeoff[0]
    print()
    print("Confusion matrix for top tradeoff")
    print(strategy)
    print("rows=true, cols=pred, order=MASK LEFT RIGHT REAR FRONT")
    print(stats["confusion"])


if __name__ == "__main__":
    main()
