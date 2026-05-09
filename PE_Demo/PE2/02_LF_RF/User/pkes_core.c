#include "pkes_core.h"

#include "gpio.h"
#include "pkes_can_app.h"
#include "pkes_can_protocol.h"
#include "pkes_lock_app.h"
#include "pkes_ranging.h"
#include "timer.h"
#include <stddef.h>

#define PKES_CORE_ANTENNA_COUNT 4u
#define PKES_CORE_LED_PULSE_TICKS 10u
#define PKES_CORE_KEY_ID 0x00000084u
#define PKES_CORE_STANDBY_HEARTBEAT_TICKS 30u
#define PKES_CORE_RSSI_SAMPLE_DEPTH 5u
#define PKES_CORE_RSSI_DUP_WINDOW_MS 80u
#define PKES_CORE_RANGING_WINDOW_MS 2000u
#define KEY01_UNLOCK_SIG 0x63u
#define KEY01_LOCK_SIG   0x62u

typedef struct
{
    uint16_t rssi;
    uint32_t timestamp_ms;
    uint8_t valid;
} pkes_core_rssi_sample_t;


typedef struct
{
    pkes_core_rssi_sample_t samples[PKES_CORE_RSSI_SAMPLE_DEPTH];
    uint8_t head;
    uint8_t count;
    uint16_t last_rssi;
    uint32_t last_time_ms;
    uint8_t last_valid;
} pkes_core_antenna_rssi_t;

typedef struct
{
    pkes_ranging_result_t result;
    uint32_t timestamp_ms;
    uint8_t valid;
} pkes_core_distance_sample_t;

typedef struct
{
    pkes_core_distance_sample_t samples[PKES_CORE_RSSI_SAMPLE_DEPTH];
    uint8_t head;
    uint8_t count;
} pkes_core_antenna_distance_t;

extern void SendLFWakeUp(void);
extern uint8_t active_antenna;

static const uint8_t s_antenna_list[PKES_CORE_ANTENNA_COUNT] = {1u, 2u, 3u, 4u};
static uint8_t s_ant_poll_idx;
static uint8_t s_led1_ticks;
static uint8_t s_led2_ticks;
static uint8_t s_led3_ticks;
static uint8_t s_idle_lf_ticks;
static uint8_t s_handle_region_code;
static uint8_t s_handle_ranging_active;
static uint8_t s_region_decision_done;
static pkes_core_antenna_rssi_t s_rssi_buffers[PKES_CORE_ANTENNA_COUNT];
static pkes_core_antenna_distance_t s_distance_buffers[PKES_CORE_ANTENNA_COUNT];

uint8_t PKES_Core_GetCanLockState(void)
{
    uint8_t lock_state = Lock_App_GetState();

    if (lock_state == PKES_LOCK_APP_LOCKING)
    {
        return PKES_LOCK_LOCKED;
    }

    return PKES_LOCK_UNLOCKED;
}

static void PKES_Core_SendStandbyState(void)
{
    CAN_App_SendSysState(PKES_SYS_STANDBY,
                         PKES_STATUS_NONE,
                         PKES_REGION_UNKNOWN,
                         0u,
                         PKES_Core_GetCanLockState(),
                         PKES_TRIGGER_NONE,
                         0u);
}

static uint16_t PKES_Core_Crc16CcittFalse(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;
    size_t i;
    uint8_t b;

    for (i = 0u; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (b = 0u; b < 8u; b++)
        {
            if ((crc & 0x8000u) != 0u)
            {
                crc = (uint16_t)((crc << 1) ^ 0x1021u);
            }
            else
            {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

/* 临时标定阶段关闭 RF/RSSI LED 脉冲指示，只保留门把手和继电器状态灯。 */
#if 0
static void PKES_Core_StartLedPulse(uint8_t led_mask)
{
    if ((led_mask & 0x01u) != 0u)
    {
        LED1_ON;
        s_led1_ticks = PKES_CORE_LED_PULSE_TICKS;
    }
    if ((led_mask & 0x02u) != 0u)
    {
        LED2_ON;
        s_led2_ticks = PKES_CORE_LED_PULSE_TICKS;
    }
    if ((led_mask & 0x04u) != 0u)
    {
        LED3_ON;
        s_led3_ticks = PKES_CORE_LED_PULSE_TICKS;
    }
}
#endif

static void PKES_Core_UpdateLedPulse(void)
{
    if (s_led1_ticks > 0u)
    {
        s_led1_ticks--;
        if (s_led1_ticks == 0u)
        {
            LED1_OFF;
        }
    }

    if (s_led2_ticks > 0u)
    {
        s_led2_ticks--;
        if (s_led2_ticks == 0u)
        {
            LED2_OFF;
        }
    }

    if (s_led3_ticks > 0u)
    {
        s_led3_ticks--;
        if (s_led3_ticks == 0u)
        {
            LED3_OFF;
        }
    }
}

static void PKES_Core_ResetRssiBuffers(void)
{
    uint8_t ant;
    uint8_t sample;

    for (ant = 0u; ant < PKES_CORE_ANTENNA_COUNT; ant++)
    {
        s_rssi_buffers[ant].head = 0u;
        s_rssi_buffers[ant].count = 0u;
        s_rssi_buffers[ant].last_rssi = 0u;
        s_rssi_buffers[ant].last_time_ms = 0u;
        s_rssi_buffers[ant].last_valid = 0u;

        for (sample = 0u; sample < PKES_CORE_RSSI_SAMPLE_DEPTH; sample++)
        {
            s_rssi_buffers[ant].samples[sample].rssi = 0u;
            s_rssi_buffers[ant].samples[sample].timestamp_ms = 0u;
            s_rssi_buffers[ant].samples[sample].valid = 0u;
        }
    }
}

static void PKES_Core_ResetDistanceBuffers(void)
{
    uint8_t ant;
    uint8_t sample;

    for (ant = 0u; ant < PKES_CORE_ANTENNA_COUNT; ant++)
    {
        s_distance_buffers[ant].head = 0u;
        s_distance_buffers[ant].count = 0u;

        for (sample = 0u; sample < PKES_CORE_RSSI_SAMPLE_DEPTH; sample++)
        {
            s_distance_buffers[ant].samples[sample].result.rssi_raw = 0u;
            s_distance_buffers[ant].samples[sample].result.distance_cm = 0u;
            s_distance_buffers[ant].samples[sample].result.distance_cm_x10 = 0u;
            s_distance_buffers[ant].samples[sample].result.antenna_id = 0u;
            s_distance_buffers[ant].samples[sample].result.region_code = PKES_REGION_UNKNOWN;
            s_distance_buffers[ant].samples[sample].result.sample_cnt = 0u;
            s_distance_buffers[ant].samples[sample].result.in_calibration_range = 0u;
            s_distance_buffers[ant].samples[sample].result.range_state = PKES_RANGING_RANGE_VALID;
            s_distance_buffers[ant].samples[sample].timestamp_ms = 0u;
            s_distance_buffers[ant].samples[sample].valid = 0u;
        }
    }
}

static uint8_t PKES_Core_IsDuplicateRssi(uint8_t ant_idx, uint16_t rssi, uint32_t now_ms)
{
    pkes_core_antenna_rssi_t *ant_buf = &s_rssi_buffers[ant_idx];

    if ((ant_buf->last_valid != 0u) &&
        (ant_buf->last_rssi == rssi) &&
        ((uint32_t)(now_ms - ant_buf->last_time_ms) <= PKES_CORE_RSSI_DUP_WINDOW_MS))
    {
        ant_buf->last_time_ms = now_ms;
        return 1u;
    }

    return 0u;
}

static void PKES_Core_StoreRssiSample(uint8_t ant_idx, uint16_t rssi, uint32_t now_ms)
{
    pkes_core_antenna_rssi_t *ant_buf = &s_rssi_buffers[ant_idx];
    pkes_core_rssi_sample_t *sample = &ant_buf->samples[ant_buf->head];

    sample->rssi = rssi;
    sample->timestamp_ms = now_ms;
    sample->valid = 1u;

    ant_buf->head = (uint8_t)((ant_buf->head + 1u) % PKES_CORE_RSSI_SAMPLE_DEPTH);
    if (ant_buf->count < PKES_CORE_RSSI_SAMPLE_DEPTH)
    {
        ant_buf->count++;
    }

    ant_buf->last_rssi = rssi;
    ant_buf->last_time_ms = now_ms;
    ant_buf->last_valid = 1u;
}

static void PKES_Core_StoreDistanceSample(uint8_t ant_idx,
                                          const pkes_ranging_result_t *result,
                                          uint32_t now_ms)
{
    pkes_core_antenna_distance_t *ant_buf = &s_distance_buffers[ant_idx];
    pkes_core_distance_sample_t *sample = &ant_buf->samples[ant_buf->head];

    if (result == NULL)
    {
        return;
    }

    sample->result = *result;
    sample->timestamp_ms = now_ms;
    sample->valid = 1u;

    ant_buf->head = (uint8_t)((ant_buf->head + 1u) % PKES_CORE_RSSI_SAMPLE_DEPTH);
    if (ant_buf->count < PKES_CORE_RSSI_SAMPLE_DEPTH)
    {
        ant_buf->count++;
    }
}

static uint8_t PKES_Core_GetLatestDistanceSample(uint8_t ant_idx,
                                                 pkes_ranging_result_t *result,
                                                 uint32_t *timestamp_ms)
{
    pkes_core_antenna_distance_t *ant_buf = &s_distance_buffers[ant_idx];
    uint8_t sample_idx;

    if ((ant_buf->count == 0u) || (result == NULL) || (timestamp_ms == NULL))
    {
        return 0u;
    }

    sample_idx = (ant_buf->head == 0u) ? (PKES_CORE_RSSI_SAMPLE_DEPTH - 1u) : (uint8_t)(ant_buf->head - 1u);
    if (ant_buf->samples[sample_idx].valid == 0u)
    {
        return 0u;
    }

    *result = ant_buf->samples[sample_idx].result;
    *timestamp_ms = ant_buf->samples[sample_idx].timestamp_ms;
    return 1u;
}

static uint8_t PKES_Core_CollectLatestDistances(pkes_ranging_result_t results[PKES_CORE_ANTENNA_COUNT],
                                                uint16_t distance_cm[PKES_CORE_ANTENNA_COUNT],
                                                uint16_t rssi_raw[PKES_CORE_ANTENNA_COUNT],
                                                uint32_t now_ms,
                                                uint8_t allow_missing,
                                                uint8_t *valid_count)
{
    uint8_t ant;
    uint8_t count = 0u;

    for (ant = 0u; ant < PKES_CORE_ANTENNA_COUNT; ant++)
    {
        uint32_t sample_time_ms;

        if ((PKES_Core_GetLatestDistanceSample(ant, &results[ant], &sample_time_ms) != 0u) &&
            ((uint32_t)(now_ms - sample_time_ms) <= PKES_CORE_RANGING_WINDOW_MS))
        {
            distance_cm[ant] = results[ant].distance_cm;
            if (rssi_raw != NULL)
            {
                rssi_raw[ant] = results[ant].rssi_raw;
            }
            count++;
            continue;
        }

        if (allow_missing == 0u)
        {
            return 0u;
        }

        results[ant].rssi_raw = PKES_RANGING_MISSING_RSSI_RAW;
        results[ant].distance_cm = PKES_RANGING_TOO_FAR_CM;
        results[ant].distance_cm_x10 = (uint16_t)(PKES_RANGING_TOO_FAR_CM * 10u);
        results[ant].antenna_id = (uint8_t)(ant + 1u);
        results[ant].region_code = PKES_REGION_UNKNOWN;
        results[ant].sample_cnt = 0u;
        results[ant].in_calibration_range = 0u;
        results[ant].range_state = PKES_RANGING_RANGE_TOO_FAR;
        distance_cm[ant] = PKES_RANGING_TOO_FAR_CM;
        if (rssi_raw != NULL)
        {
            rssi_raw[ant] = PKES_RANGING_MISSING_RSSI_RAW;
        }
    }

    if (valid_count != NULL)
    {
        *valid_count = count;
    }

    return 1u;
}

static void PKES_Core_SendFusedDistances(const pkes_ranging_result_t results[PKES_CORE_ANTENNA_COUNT],
                                         uint8_t region_code)
{
    uint8_t ant;

    for (ant = 0u; ant < PKES_CORE_ANTENNA_COUNT; ant++)
    {
        CAN_App_SendDistance(results[ant].antenna_id,
                             results[ant].distance_cm,
                             region_code);
    }
}

static uint8_t PKES_Core_BuildDistanceRegionFlags(uint8_t distance_valid, uint8_t region_code)
{
    uint8_t flags = 0u;

    if (distance_valid != 0u)
    {
        flags |= PKES_FLAG_DISTANCE_VALID;
    }
    if (region_code != PKES_REGION_UNKNOWN)
    {
        flags |= PKES_FLAG_REGION_VALID;
    }

    return flags;
}

static void PKES_Core_SendRegionDoneState(uint8_t region_code, uint8_t distance_valid)
{
    CAN_App_SendSysState(PKES_SYS_REGION_DONE,
                         (region_code == PKES_REGION_UNKNOWN) ? PKES_STATUS_REGION_INVALID : PKES_STATUS_REGION_VALID,
                         region_code,
                         0u,
                         PKES_Core_GetCanLockState(),
                         PKES_TRIGGER_HANDLE,
                         PKES_Core_BuildDistanceRegionFlags(distance_valid, region_code));
}

static void PKES_Core_ApplyHandleLockDecision(uint8_t region_code, uint8_t distance_valid)
{
    uint8_t lock_state = Lock_App_GetState();
    uint8_t unlock_allowed = 0u;

    if ((s_handle_region_code == PKES_REGION_LEFT_DOOR) &&
        (region_code == PKES_REGION_LEFT_DOOR))
    {
        unlock_allowed = 1u;
    }
    else if (region_code == PKES_REGION_INSIDE)
    {
        unlock_allowed = 1u;
    }
    else if ((s_handle_region_code == PKES_REGION_RIGHT_DOOR) &&
             (region_code == PKES_REGION_RIGHT_DOOR))
    {
        unlock_allowed = 1u;
    }

    if (unlock_allowed != 0u)
    {
        if (lock_state != PKES_LOCK_APP_UNLOCKING)
        {
            Lock_App_UnlockPulse();
        }
        CAN_App_SendSysState(PKES_SYS_UNLOCK_DONE,
                             PKES_STATUS_UNLOCK_DONE,
                             region_code,
                             0u,
                             PKES_LOCK_UNLOCKED,
                             PKES_TRIGGER_HANDLE,
                             PKES_Core_BuildDistanceRegionFlags(distance_valid, region_code));
    }
    else
    {
        if (lock_state != PKES_LOCK_APP_LOCKING)
        {
            Lock_App_LockPulse();
        }
        CAN_App_SendSysState(PKES_SYS_LOCK_DONE,
                             PKES_STATUS_LOCK_DONE,
                             region_code,
                             0u,
                             PKES_LOCK_LOCKED,
                             PKES_TRIGGER_HANDLE,
                             PKES_Core_BuildDistanceRegionFlags(distance_valid, region_code));
    }
}

static void PKES_Core_TryHandleRegionDecision(uint32_t now_ms)//最终区域判断和开闭锁决策的函数
{
    pkes_ranging_result_t results[PKES_CORE_ANTENNA_COUNT];
    uint16_t distance_cm[PKES_CORE_ANTENNA_COUNT];
    uint16_t rssi_raw[PKES_CORE_ANTENNA_COUNT];
    uint8_t region_code;
    uint8_t valid_count;

    if ((s_handle_ranging_active == 0u) || (s_region_decision_done != 0u))
    {
        return;
    }

    if (PKES_Core_CollectLatestDistances(results, distance_cm, rssi_raw, now_ms, 0u, &valid_count) == 0u)
    {
        return;
    }

    region_code = PKES_Ranging_DecideRegionFromRssi(rssi_raw, distance_cm);//调用判区算法
    PKES_Core_SendFusedDistances(results, region_code);//把 4 根天线的距离结果通过 CAN 发出去。
    PKES_Core_SendRegionDoneState(region_code, 1u);//通过 CAN 上报“区域判断完成”。
    PKES_Core_ApplyHandleLockDecision(region_code, 1u);//根据判出来的区域执行开锁或闭锁。
    s_region_decision_done = 1u;
}

static void PKES_Core_HandleRangingSample(uint8_t ant_array_idx,
                                          uint8_t antenna_id,
                                          uint16_t rssi,
                                          uint32_t now_ms)//被动开锁路径里“收到钥匙 RSSI 回包后”的核心处理函数。
{
    pkes_ranging_result_t ranging_result;
    uint8_t next_sample_cnt = (uint8_t)(s_distance_buffers[ant_array_idx].count + 1u);

    if (next_sample_cnt > PKES_CORE_RSSI_SAMPLE_DEPTH)
    {
        next_sample_cnt = PKES_CORE_RSSI_SAMPLE_DEPTH;
    }//限制最大值，因此我最大只标定到了180cm

    PKES_Ranging_Evaluate(antenna_id,
                          rssi,
                          next_sample_cnt,
                          PKES_REGION_UNKNOWN,
                          &ranging_result);//函数会把 RSSI 转成距离，填入 ranging_result：

    PKES_Core_StoreDistanceSample(ant_array_idx, &ranging_result, now_ms);//保存这个距离样本。它会把结果存到对应天线的环形缓存里。每根天线最多保存 5 个最近距离样本。
    CAN_App_SendSysState(PKES_SYS_SINGLE_DISTANCE_DONE,
                         PKES_STATUS_DISTANCE_VALID,
                         PKES_REGION_UNKNOWN,
                         antenna_id,
                         PKES_Core_GetCanLockState(),
                         PKES_TRIGGER_HANDLE,
                         (uint8_t)(PKES_FLAG_RSSI_VALID | PKES_FLAG_DISTANCE_VALID));// CAN 上报单天线距离完成
		
    PKES_Core_TryHandleRegionDecision(now_ms);//它会尝试做最终四天线判区。
}

static void PKES_Core_HandleActiveKeyFrame(const uint8_t buf[5])//主动遥控钥匙帧处理函数
{
    uint8_t sig1 = buf[1];
    uint8_t sig2 = buf[2];

    if (sig1 == KEY01_UNLOCK_SIG)//主动开锁
    {
        CAN_App_SendSysState(PKES_SYS_TRIGGER_DETECTED,
                             PKES_STATUS_NONE,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_RF_UNLOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
        CAN_App_SendKeyRssi((uint8_t)PKES_CORE_KEY_ID,
                            0u,
                            0u,
                            PKES_CMD_ACTIVE_RF_UNLOCK);
        CAN_App_SendSysState(PKES_SYS_ID_AUTH_OK,
                             PKES_STATUS_UNLOCK_CMD,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_RF_UNLOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));

        Lock_App_UnlockPulse();//开锁函数
			
        /* 临时关闭遥控器开锁 LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x07u); */

        CAN_App_SendSysState(PKES_SYS_UNLOCK_DONE,
                             PKES_STATUS_UNLOCK_DONE,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_LOCK_UNLOCKED,
                             PKES_TRIGGER_RF_UNLOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
    }
    else if (sig2 == KEY01_LOCK_SIG)
    {
        CAN_App_SendSysState(PKES_SYS_TRIGGER_DETECTED,
                             PKES_STATUS_NONE,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_RF_LOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
        CAN_App_SendKeyRssi((uint8_t)PKES_CORE_KEY_ID,
                            0u,
                            0u,
                            PKES_CMD_ACTIVE_RF_LOCK);
        CAN_App_SendSysState(PKES_SYS_ID_AUTH_OK,
                             PKES_STATUS_LOCK_CMD,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_RF_LOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));

        Lock_App_LockPulse();//闭锁函数
			
        /* 临时关闭遥控器闭锁 LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x06u); */

        CAN_App_SendSysState(PKES_SYS_LOCK_DONE,
                             PKES_STATUS_LOCK_DONE,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_LOCK_LOCKED,
                             PKES_TRIGGER_RF_LOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
    }
    else
    {
        CAN_App_SendSysState(PKES_SYS_TRIGGER_DETECTED,
                             PKES_STATUS_NONE,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_NONE,
                             PKES_FLAG_CRC_OK);
        CAN_App_SendSysState(PKES_SYS_ID_AUTH_FAIL,
                             PKES_STATUS_UNKNOWN_ID,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_Core_GetCanLockState(),
                             PKES_TRIGGER_NONE,
                             PKES_FLAG_CRC_OK);
    }
}

static void PKES_Core_HandlePassiveRssiFrame(const uint8_t buf[5])//被动 RSSI 回包处理函数
{
	//这里解析 RF 帧：
    uint8_t ant_id = buf[0];
    uint16_t rssi_val = (uint16_t)(((uint16_t)buf[2] << 8) | buf[1]);
    uint8_t ant_idx = (uint8_t)(ant_id - 0xA1u);
    uint8_t ant_array_idx = (uint8_t)(ant_idx - 1u);
    uint32_t now_ms = TIMER_GetMillis();

    if (ant_array_idx >= PKES_CORE_ANTENNA_COUNT)
    {
        return;
    }

    if (PKES_Core_IsDuplicateRssi(ant_array_idx, rssi_val, now_ms) != 0u)
    {
        return;
    }

    PKES_Core_StoreRssiSample(ant_array_idx, rssi_val, now_ms);
    CAN_App_SendKeyRssi((uint8_t)PKES_CORE_KEY_ID,
                        rssi_val,
                        ant_idx,
                        PKES_CMD_PASSIVE_RESPONSE);
    CAN_App_SendSysState(PKES_SYS_RF_RECEIVED,
                         PKES_STATUS_FRAME_VALID,
                         PKES_REGION_UNKNOWN,
                         ant_idx,
                         PKES_Core_GetCanLockState(),
                         PKES_TRIGGER_HANDLE,
                         (uint8_t)(PKES_FLAG_CRC_OK | PKES_FLAG_RSSI_VALID));
    CAN_App_SendSysState(PKES_SYS_RSSI_PROCESS,
                         PKES_STATUS_FRAME_VALID,
                         PKES_REGION_UNKNOWN,
                         ant_idx,
                         PKES_Core_GetCanLockState(),
                         PKES_TRIGGER_HANDLE,
                         (uint8_t)(PKES_FLAG_CRC_OK | PKES_FLAG_RSSI_VALID));
		
    PKES_Core_HandleRangingSample(ant_array_idx, ant_idx, rssi_val, now_ms);//正儿八经的测距处理判定区域

    if (ant_id == 0xA2u)
    {
        /* 临时关闭天线1 RSSI LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x01u); */
    }
    if (ant_id == 0xA3u)
    {
        /* 临时关闭天线2 RSSI LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x02u); */
    }
    if (ant_id == 0xA4u)
    {
        /* 临时关闭天线3 RSSI LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x04u); */
    }
    if (ant_id == 0xA5u)
    {
        /* 临时关闭天线4 RSSI LED 指示，只保留门把手触发点灯。 */
        /* PKES_Core_StartLedPulse(0x05u); */
    }
}

void PKES_Core_Init(void)
{
    s_ant_poll_idx = 0u;
    s_led1_ticks = 0u;
    s_led2_ticks = 0u;
    s_led3_ticks = 0u;
    s_idle_lf_ticks = 0u;
    s_handle_region_code = PKES_REGION_UNKNOWN;
    s_handle_ranging_active = 0u;
    s_region_decision_done = 0u;
    PKES_Core_ResetRssiBuffers();
    PKES_Core_ResetDistanceBuffers();
    active_antenna = s_antenna_list[s_ant_poll_idx];

    PKES_Core_SendStandbyState();
}

void PKES_Core_StartHandleRanging(uint8_t region_code)
{
    s_handle_region_code = region_code;
    s_handle_ranging_active = 1u;
    s_region_decision_done = 0u;
    PKES_Core_ResetRssiBuffers();
    PKES_Core_ResetDistanceBuffers();
}

void PKES_Core_EndHandleRanging(void)
{
    s_handle_region_code = PKES_REGION_UNKNOWN;
    s_handle_ranging_active = 0u;
    s_region_decision_done = 0u;
}

void PKES_Core_FinalizeHandleRanging(uint32_t now_ms)
{
    pkes_ranging_result_t results[PKES_CORE_ANTENNA_COUNT];
    uint16_t distance_cm[PKES_CORE_ANTENNA_COUNT];
    uint16_t rssi_raw[PKES_CORE_ANTENNA_COUNT];
    uint8_t region_code;
    uint8_t valid_count;

    if ((s_handle_ranging_active == 0u) || (s_region_decision_done != 0u))
    {
        return;
    }

    if (PKES_Core_CollectLatestDistances(results, distance_cm, rssi_raw, now_ms, 1u, &valid_count) == 0u)//允许缺失天线。缺失天线会被补成：180cm
    {
        return;
    }

    region_code = (valid_count == 0u) ? PKES_REGION_UNKNOWN : PKES_Ranging_DecideRegionFromRssi(rssi_raw, distance_cm);
    PKES_Core_SendFusedDistances(results, region_code);
    PKES_Core_SendRegionDoneState(region_code, (valid_count == 0u) ? 0u : 1u);
    PKES_Core_ApplyHandleLockDecision(region_code, (valid_count == 0u) ? 0u : 1u);
    s_region_decision_done = 1u;
}

void PKES_Core_HandleLFEvent(void)
{
    SendLFWakeUp();
}

void PKES_Core_UpdateTick(void)
{
    PKES_Core_UpdateLedPulse();
    Lock_App_UpdateTick();

    if (s_idle_lf_ticks < PKES_CORE_STANDBY_HEARTBEAT_TICKS)
    {
        s_idle_lf_ticks++;
    }

    if (s_idle_lf_ticks >= PKES_CORE_STANDBY_HEARTBEAT_TICKS)
    {
        PKES_Core_SendStandbyState();
        s_idle_lf_ticks = 0u;
    }
}

void PKES_Core_ProcessRFData(const uint8_t buf[5])//按 buf[0] 分两类,分主动开锁和被动开锁
{
    uint16_t crc_calc = PKES_Core_Crc16CcittFalse(buf, 3u);
    uint16_t crc_rx = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);

    s_idle_lf_ticks = 0u;

	if (crc_calc != crc_rx)//如果RCR校验不通过，直接return
    {
        if (buf[0] == 0x84u)
        {
            CAN_App_SendSysState(PKES_SYS_TRIGGER_DETECTED,
                                 PKES_STATUS_NONE,
                                 PKES_REGION_UNKNOWN,
                                 0u,
                                 PKES_Core_GetCanLockState(),
                                 PKES_TRIGGER_NONE,
                                 0u);
            CAN_App_SendSysState(PKES_SYS_ID_AUTH_FAIL,
                                 PKES_STATUS_CRC_ERROR,
                                 PKES_REGION_UNKNOWN,
                                 0u,
                                 PKES_Core_GetCanLockState(),
                                 PKES_TRIGGER_NONE,
                                 0u);
        }
        return;
    }

    if (buf[0] == 0x84u)//主动遥控帧
    {
        PKES_Core_HandleActiveKeyFrame(buf);
    }
    else if ((buf[0] >= 0xA2u) && (buf[0] <= 0xA5u))//处理被动 RSSI 回包
    {
        PKES_Core_HandlePassiveRssiFrame(buf);
    }
}
