#include "pkes_ranging.h"

#include <stddef.h>

#define PKES_RANGING_TABLE_SIZE 29u

typedef struct
{
    uint16_t distance_cm_x10;
    uint16_t rssi_x2;
} pkes_ranging_point_t;

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

uint8_t PKES_Ranging_DecideRegion(uint8_t region_hint, uint16_t distance_cm)
{
    (void)region_hint;
    (void)distance_cm;

    /* 区域/距离判断策略暂未标定，0x303 Byte3 暂时固定上报 0x00。 */
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
