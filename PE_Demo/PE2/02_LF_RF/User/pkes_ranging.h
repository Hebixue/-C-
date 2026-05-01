#ifndef PKES_RANGING_H_
#define PKES_RANGING_H_

#include "pkes_can_protocol.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PKES_RANGING_MIN_CM 9u
#define PKES_RANGING_MAX_CM 179u
#define PKES_RANGING_TOO_FAR_CM 180u

/* Temporary region thresholds. Recalibrate these with vehicle-level requirements. */
#define PKES_RANGING_NEAR_CM 80u
#define PKES_RANGING_BOUNDARY_CM 120u

typedef enum
{
    PKES_RANGING_RANGE_VALID = 0u,
    PKES_RANGING_RANGE_TOO_NEAR = 1u,
    PKES_RANGING_RANGE_TOO_FAR = 2u
} pkes_ranging_range_state_t;

typedef struct
{
    uint16_t rssi_raw;
    uint16_t distance_cm;
    uint16_t distance_cm_x10;
    uint8_t antenna_id;
    uint8_t region_code;
    uint8_t sample_cnt;
    uint8_t in_calibration_range;
    uint8_t range_state;
} pkes_ranging_result_t;

/* Convert RSSI to distance with the 4-antenna median table, unit: 0.1 cm. */
uint16_t PKES_Ranging_RssiToDistanceCmX10(uint16_t rssi_raw);

/* Convert RSSI to rounded distance in cm. */
uint16_t PKES_Ranging_RssiToDistanceCm(uint16_t rssi_raw);

/* Return nonzero if RSSI is within the current 9~179 cm calibration range. */
uint8_t PKES_Ranging_IsRssiInCalibrationRange(uint16_t rssi_raw);

/* Return pkes_ranging_range_state_t for the current calibration range. */
uint8_t PKES_Ranging_GetRangeState(uint16_t rssi_raw);

/* Decide a 0x303 region code from distance and an optional business/handle hint. */
uint8_t PKES_Ranging_DecideRegion(uint8_t region_hint, uint16_t distance_cm);

/* Full ranging path: RSSI -> distance -> region. */
void PKES_Ranging_Evaluate(uint8_t antenna_id,
                           uint16_t rssi_raw,
                           uint8_t sample_cnt,
                           uint8_t region_hint,
                           pkes_ranging_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* PKES_RANGING_H_ */
