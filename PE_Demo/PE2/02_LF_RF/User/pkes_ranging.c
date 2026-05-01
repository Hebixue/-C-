#include "pkes_ranging.h"

#include <stddef.h>

#define PKES_RANGING_TABLE_SIZE 29u
#define PKES_RANGING_TEMPLATE_ZONE_COUNT 5u
#define PKES_RANGING_TEMPLATE_K 9u
#define PKES_RANGING_TEMPLATE_ACCEPT_PERCENT 85u
#define PKES_RANGING_INSIDE_K 9u
#define PKES_RANGING_INSIDE_ACCEPT_PERCENT 80u
#define PKES_RANGING_INSIDE_MAX_CM 135u
#define PKES_RANGING_SCORE_INVALID 0xFFFFFFFFu

typedef struct
{
    uint16_t distance_cm_x10;
    uint16_t rssi_x2;
} pkes_ranging_point_t;

typedef struct
{
    uint16_t rssi[4];
    uint8_t region_code;
} pkes_rssi_template_t;

#include "pkes_rssi_template_table_5cm.h"
#include "pkes_inside_template_table_3cm.h"

/* 4-antenna RSSI median calibration table with measured midpoint corrections.
 * RSSI is stored as x2 to represent half-count points without floating point.
 */
static const pkes_ranging_point_t s_rssi_distance_table[PKES_RANGING_TABLE_SIZE] =
{
    {90u, 5453u},
    {140u, 5124u},
    {190u, 4507u},
    {240u, 4418u},
    {290u, 4343u},
    {340u, 3700u},
    {390u, 3472u},
    {440u, 3280u},
    {490u, 3126u},
    {540u, 3013u},
    {590u, 2909u},
    {640u, 2349u},
    {690u, 2219u},
    {740u, 2118u},
    {790u, 2043u},
    {840u, 1955u},
    {890u, 1884u},
    {940u, 1818u},
    {990u, 1738u},
    {1040u, 1702u},
    {1090u, 1630u},
    {1140u, 1572u},
    {1190u, 1527u},
    {1240u, 1492u},
    {1290u, 1458u},
    {1390u, 1370u},
    {1490u, 1310u},
    {1590u, 1249u},
    {1790u, 1232u}
};

static uint16_t PKES_Ranging_ClampDistanceCmX10(uint16_t rssi_x2)
{
    if (rssi_x2 >= s_rssi_distance_table[0].rssi_x2)
    {
        return s_rssi_distance_table[0].distance_cm_x10;
    }

    return (uint16_t)(PKES_RANGING_TOO_FAR_CM * 10u);
}

uint8_t PKES_Ranging_IsRssiInCalibrationRange(uint16_t rssi_raw)
{
    return (PKES_Ranging_GetRangeState(rssi_raw) == PKES_RANGING_RANGE_VALID) ? 1u : 0u;
}

uint8_t PKES_Ranging_GetRangeState(uint16_t rssi_raw)
{
    uint16_t rssi_x2 = (uint16_t)(rssi_raw * 2u);

    if (rssi_x2 > s_rssi_distance_table[0].rssi_x2)
    {
        return PKES_RANGING_RANGE_TOO_NEAR;
    }

    if (rssi_x2 < s_rssi_distance_table[PKES_RANGING_TABLE_SIZE - 1u].rssi_x2)
    {
        return PKES_RANGING_RANGE_TOO_FAR;
    }

    return PKES_RANGING_RANGE_VALID;
}

uint16_t PKES_Ranging_RssiToDistanceCmX10(uint16_t rssi_raw)
{
    uint16_t rssi_x2 = (uint16_t)(rssi_raw * 2u);
    uint8_t i;

    if (PKES_Ranging_IsRssiInCalibrationRange(rssi_raw) == 0u)
    {
        return PKES_Ranging_ClampDistanceCmX10(rssi_x2);
    }

    for (i = 0u; i < (PKES_RANGING_TABLE_SIZE - 1u); i++)
    {
        uint16_t rssi_hi = s_rssi_distance_table[i].rssi_x2;
        uint16_t rssi_lo = s_rssi_distance_table[i + 1u].rssi_x2;

        if ((rssi_x2 <= rssi_hi) && (rssi_x2 >= rssi_lo))
        {
            uint16_t dist_hi = s_rssi_distance_table[i].distance_cm_x10;
            uint16_t dist_lo = s_rssi_distance_table[i + 1u].distance_cm_x10;
            uint32_t rssi_span = (uint32_t)(rssi_hi - rssi_lo);
            uint32_t rssi_delta = (uint32_t)(rssi_hi - rssi_x2);
            uint32_t dist_span = (uint32_t)(dist_lo - dist_hi);

            if (rssi_span == 0u)
            {
                return dist_hi;
            }

            return (uint16_t)(dist_hi + (uint16_t)(((rssi_delta * dist_span) + (rssi_span / 2u)) / rssi_span));
        }
    }

    return s_rssi_distance_table[PKES_RANGING_TABLE_SIZE - 1u].distance_cm_x10;
}

uint16_t PKES_Ranging_RssiToDistanceCm(uint16_t rssi_raw)
{
    uint16_t distance_cm_x10 = PKES_Ranging_RssiToDistanceCmX10(rssi_raw);

    return (uint16_t)((distance_cm_x10 + 5u) / 10u);
}

static uint32_t PKES_Ranging_AbsDiffU16(uint16_t a, uint16_t b)
{
    return (a >= b) ? (uint32_t)(a - b) : (uint32_t)(b - a);
}

static void PKES_Ranging_InsertBestScore(uint32_t best_scores[PKES_RANGING_TEMPLATE_K],
                                         uint32_t score)
{
    uint8_t i;
    uint8_t max_idx = 0u;
    uint32_t max_score = best_scores[0];

    for (i = 1u; i < PKES_RANGING_TEMPLATE_K; i++)
    {
        if (best_scores[i] > max_score)
        {
            max_score = best_scores[i];
            max_idx = i;
        }
    }

    if (score < max_score)
    {
        best_scores[max_idx] = score;
    }
}

static uint32_t PKES_Ranging_SumBestScores(const uint32_t best_scores[PKES_RANGING_TEMPLATE_K])
{
    uint8_t i;
    uint32_t sum = 0u;

    for (i = 0u; i < PKES_RANGING_TEMPLATE_K; i++)
    {
        if (best_scores[i] == PKES_RANGING_SCORE_INVALID)
        {
            return PKES_RANGING_SCORE_INVALID;
        }
        sum += best_scores[i];
    }

    return sum;
}

static uint32_t PKES_Ranging_TemplateRawSumScore4(const uint16_t rssi_raw[4],
                                                  const pkes_rssi_template_t *tmpl)
{
    uint32_t raw_score;
    uint32_t sample_sum;
    uint32_t tmpl_sum;

    raw_score = PKES_Ranging_AbsDiffU16(rssi_raw[0], tmpl->rssi[0]) +
                PKES_Ranging_AbsDiffU16(rssi_raw[1], tmpl->rssi[1]) +
                PKES_Ranging_AbsDiffU16(rssi_raw[2], tmpl->rssi[2]) +
                PKES_Ranging_AbsDiffU16(rssi_raw[3], tmpl->rssi[3]);

    sample_sum = (uint32_t)rssi_raw[0] + (uint32_t)rssi_raw[1] +
                 (uint32_t)rssi_raw[2] + (uint32_t)rssi_raw[3];
    tmpl_sum = (uint32_t)tmpl->rssi[0] + (uint32_t)tmpl->rssi[1] +
               (uint32_t)tmpl->rssi[2] + (uint32_t)tmpl->rssi[3];

    /* raw_sum feature: [A1,A2,A3,A4, 0.25 * sum]. Multiply by 4 to stay integer. */
    return (raw_score * 4u) + ((sample_sum >= tmpl_sum) ? (sample_sum - tmpl_sum) : (tmpl_sum - sample_sum));
}

static uint32_t PKES_Ranging_TemplateRawScore(const uint16_t rssi_raw[4],
                                              const uint16_t tmpl_rssi[4])
{
    return PKES_Ranging_AbsDiffU16(rssi_raw[0], tmpl_rssi[0]) +
           PKES_Ranging_AbsDiffU16(rssi_raw[1], tmpl_rssi[1]) +
           PKES_Ranging_AbsDiffU16(rssi_raw[2], tmpl_rssi[2]) +
           PKES_Ranging_AbsDiffU16(rssi_raw[3], tmpl_rssi[3]);
}

static uint8_t PKES_Ranging_IsInsideByTemplate(const uint16_t rssi_raw[4],
                                               const uint16_t distance_cm[4])
{
    uint32_t inside_best[PKES_RANGING_INSIDE_K];
    uint32_t outside_best[PKES_RANGING_INSIDE_K];
    uint32_t inside_score;
    uint32_t outside_score;
    uint16_t tmpl_idx;
    uint8_t i;

    if ((distance_cm[0] > PKES_RANGING_INSIDE_MAX_CM) ||
        (distance_cm[1] > PKES_RANGING_INSIDE_MAX_CM) ||
        (distance_cm[2] > PKES_RANGING_INSIDE_MAX_CM) ||
        (distance_cm[3] > PKES_RANGING_INSIDE_MAX_CM))
    {
        return 0u;
    }

    for (i = 0u; i < PKES_RANGING_INSIDE_K; i++)
    {
        inside_best[i] = PKES_RANGING_SCORE_INVALID;
        outside_best[i] = PKES_RANGING_SCORE_INVALID;
    }

    for (tmpl_idx = 0u; tmpl_idx < PKES_INSIDE_TEMPLATE_3CM_COUNT; tmpl_idx++)
    {
        uint32_t score = PKES_Ranging_TemplateRawScore(rssi_raw, s_pkes_inside_template_3cm[tmpl_idx]);
        PKES_Ranging_InsertBestScore(inside_best, score);
    }

    for (tmpl_idx = 0u; tmpl_idx < PKES_RSSI_TEMPLATE_5CM_COUNT; tmpl_idx++)
    {
        const pkes_rssi_template_t *tmpl = &s_pkes_rssi_template_5cm[tmpl_idx];
        uint32_t score = PKES_Ranging_TemplateRawScore(rssi_raw, tmpl->rssi);
        PKES_Ranging_InsertBestScore(outside_best, score);
    }

    inside_score = PKES_Ranging_SumBestScores(inside_best);
    outside_score = PKES_Ranging_SumBestScores(outside_best);

    if ((inside_score == PKES_RANGING_SCORE_INVALID) ||
        (outside_score == PKES_RANGING_SCORE_INVALID))
    {
        return 0u;
    }

    return ((inside_score * 100u) <= (outside_score * PKES_RANGING_INSIDE_ACCEPT_PERCENT)) ? 1u : 0u;
}

uint8_t PKES_Ranging_DecideRegionFromRssi(const uint16_t rssi_raw[4],
                                          const uint16_t distance_cm[4])
{
    uint32_t best_scores[PKES_RANGING_TEMPLATE_ZONE_COUNT][PKES_RANGING_TEMPLATE_K];
    uint32_t zone_scores[PKES_RANGING_TEMPLATE_ZONE_COUNT];
    uint32_t best_zone_score = PKES_RANGING_SCORE_INVALID;
    uint8_t best_region = PKES_REGION_UNKNOWN;
    uint8_t zone;
    uint8_t i;
    uint16_t tmpl_idx;

    if ((rssi_raw == NULL) || (distance_cm == NULL))
    {
        return PKES_REGION_UNKNOWN;
    }

    if (PKES_Ranging_IsInsideByTemplate(rssi_raw, distance_cm) != 0u)
    {
        return PKES_REGION_INSIDE;
    }

    for (zone = 0u; zone < PKES_RANGING_TEMPLATE_ZONE_COUNT; zone++)
    {
        for (i = 0u; i < PKES_RANGING_TEMPLATE_K; i++)
        {
            best_scores[zone][i] = PKES_RANGING_SCORE_INVALID;
        }
    }

    for (tmpl_idx = 0u; tmpl_idx < PKES_RSSI_TEMPLATE_5CM_COUNT; tmpl_idx++)
    {
        const pkes_rssi_template_t *tmpl = &s_pkes_rssi_template_5cm[tmpl_idx];
        uint8_t region = tmpl->region_code;
        uint32_t score;

        if (region >= PKES_RANGING_TEMPLATE_ZONE_COUNT)
        {
            continue;
        }

        score = PKES_Ranging_TemplateRawSumScore4(rssi_raw, tmpl);
        PKES_Ranging_InsertBestScore(best_scores[region], score);
    }

    for (zone = 0u; zone < PKES_RANGING_TEMPLATE_ZONE_COUNT; zone++)
    {
        zone_scores[zone] = PKES_Ranging_SumBestScores(best_scores[zone]);
        if (zone_scores[zone] < best_zone_score)
        {
            best_zone_score = zone_scores[zone];
            best_region = zone;
        }
    }

    if ((best_zone_score == PKES_RANGING_SCORE_INVALID) ||
        (best_region == PKES_REGION_UNKNOWN))
    {
        return PKES_REGION_UNKNOWN;
    }

    /* Accept valid zones only when best valid score is at least 15% better than MASK. */
    if ((best_zone_score * 100u) > (zone_scores[PKES_REGION_UNKNOWN] * PKES_RANGING_TEMPLATE_ACCEPT_PERCENT))
    {
        return PKES_REGION_UNKNOWN;
    }

    return best_region;
}

uint8_t PKES_Ranging_DecideRegion(uint8_t region_hint, uint16_t distance_cm)
{
    (void)region_hint;
    (void)distance_cm;

    /* 区域/距离判断策略暂未标定，0x303 Byte3 暂时固定上报 0x00。 */
    return PKES_REGION_UNKNOWN;
}

static int16_t PKES_Ranging_AxisState(int16_t value_cm)
{
    if (value_cm > (int16_t)PKES_RANGING_AXIS_SIDE_CM)
    {
        return 1;
    }
    if (value_cm < (int16_t)(-(int16_t)PKES_RANGING_AXIS_SIDE_CM))
    {
        return -1;
    }
    if ((value_cm <= (int16_t)PKES_RANGING_AXIS_CENTER_CM) &&
        (value_cm >= (int16_t)(-(int16_t)PKES_RANGING_AXIS_CENTER_CM)))
    {
        return 0;
    }

    return 2;
}

uint8_t PKES_Ranging_DecideRegionFromDistances(const uint16_t distance_cm[4])
{
    int16_t x_feature_cm;
    int16_t rear_feature_cm;
    int16_t x_state;
    int16_t y_state;

    if (distance_cm == NULL)
    {
        return PKES_REGION_UNKNOWN;
    }

    /* Ant1=left, Ant2=right. Positive x means closer to right side. */
    x_feature_cm = (int16_t)((int16_t)distance_cm[0] - (int16_t)distance_cm[1]);
    /* Ant3=front, Ant4=rear. Positive y means closer to rear side. */
    rear_feature_cm = (int16_t)((int16_t)distance_cm[2] - (int16_t)distance_cm[3]);

    x_state = PKES_Ranging_AxisState(x_feature_cm);
    y_state = PKES_Ranging_AxisState(rear_feature_cm);

    if ((x_state == 2) || (y_state == 2))
    {
        return PKES_REGION_UNKNOWN;
    }

    if ((x_state == 0) && (y_state == 0))
    {
        return PKES_REGION_INSIDE;
    }
    if ((x_state < 0) && (y_state == 0))
    {
        return PKES_REGION_LEFT_DOOR;
    }
    if ((x_state > 0) && (y_state == 0))
    {
        return PKES_REGION_RIGHT_DOOR;
    }
    if ((x_state == 0) && (y_state > 0))
    {
        return PKES_REGION_TRUNK;
    }
    if ((x_state == 0) && (y_state < 0))
    {
        return PKES_REGION_FRONT;
    }

    /* Diagonal vertex-adjacent zones are masked. */
    return PKES_REGION_UNKNOWN;
}

void PKES_Ranging_Evaluate(uint8_t antenna_id,
                           uint16_t rssi_raw,
                           uint8_t sample_cnt,
                           uint8_t region_hint,
                           pkes_ranging_result_t *result)
{
    if (result == NULL)
    {
        return;
    }

    result->rssi_raw = rssi_raw;
    result->antenna_id = antenna_id;
    result->sample_cnt = sample_cnt;
    result->distance_cm_x10 = PKES_Ranging_RssiToDistanceCmX10(rssi_raw);
    result->distance_cm = (uint16_t)((result->distance_cm_x10 + 5u) / 10u);
    result->region_code = PKES_Ranging_DecideRegion(region_hint, result->distance_cm);
    result->range_state = PKES_Ranging_GetRangeState(rssi_raw);
    result->in_calibration_range = (result->range_state == PKES_RANGING_RANGE_VALID) ? 1u : 0u;
}
