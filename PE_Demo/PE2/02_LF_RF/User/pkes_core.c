#include "pkes_core.h"

#include "gpio.h"
#include "pkes_can_app.h"
#include "pkes_can_protocol.h"
#include "pkes_lock_app.h"
#include "timer.h"
#include <stddef.h>

#define PKES_CORE_ANTENNA_COUNT 4u
#define PKES_CORE_LED_PULSE_TICKS 10u
#define PKES_CORE_KEY_ID 0x00000084u
#define PKES_CORE_STANDBY_HEARTBEAT_TICKS 30u
#define PKES_CORE_RSSI_SAMPLE_DEPTH 5u
#define PKES_CORE_RSSI_DUP_WINDOW_MS 80u

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
    uint8_t duplicate_count;
} pkes_core_antenna_rssi_t;

extern void SendLFWakeUp(void);
extern uint8_t active_antenna;

static const uint8_t s_antenna_list[PKES_CORE_ANTENNA_COUNT] = {1u, 2u, 3u, 4u};
static uint8_t s_ant_poll_idx;
static uint8_t s_led1_ticks;
static uint8_t s_led2_ticks;
static uint8_t s_led3_ticks;
static uint8_t s_idle_lf_ticks;
static pkes_core_antenna_rssi_t s_rssi_buffers[PKES_CORE_ANTENNA_COUNT];

static void PKES_Core_SendStandbyState(void)
{
    CAN_App_SendSysState(PKES_SYS_STANDBY,
                         PKES_STATUS_NONE,
                         PKES_REGION_UNKNOWN,
                         0u,
                         PKES_LOCK_KEEP,
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
        s_rssi_buffers[ant].duplicate_count = 0u;

        for (sample = 0u; sample < PKES_CORE_RSSI_SAMPLE_DEPTH; sample++)
        {
            s_rssi_buffers[ant].samples[sample].rssi = 0u;
            s_rssi_buffers[ant].samples[sample].timestamp_ms = 0u;
            s_rssi_buffers[ant].samples[sample].valid = 0u;
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
        if (ant_buf->duplicate_count < 0xFFu)
        {
            ant_buf->duplicate_count++;
        }
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
    ant_buf->duplicate_count = 0u;
}

static void PKES_Core_HandleActiveKeyFrame(const uint8_t buf[5])
{
    uint8_t sig1 = buf[1];
    uint8_t sig2 = buf[2];

    if ((sig2 == 0xFFu) && (sig1 != 0xFFu))
    {
        Lock_App_UnlockPulse();
        PKES_Core_StartLedPulse(0x07u);

        CAN_App_SendSysState(PKES_SYS_UNLOCK_DONE,
                             PKES_STATUS_UNLOCK_CMD,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_LOCK_UNLOCKED,
                             PKES_TRIGGER_RF_UNLOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
    }
    else if ((sig1 == 0xFFu) && (sig2 != 0xFFu))
    {
        Lock_App_LockPulse();
        PKES_Core_StartLedPulse(0x06u);

        CAN_App_SendSysState(PKES_SYS_LOCK_DONE,
                             PKES_STATUS_LOCK_CMD,
                             PKES_REGION_UNKNOWN,
                             0u,
                             PKES_LOCK_LOCKED,
                             PKES_TRIGGER_RF_LOCK,
                             (uint8_t)(PKES_FLAG_ID_OK | PKES_FLAG_CRC_OK));
    }
}

static void PKES_Core_HandlePassiveRssiFrame(const uint8_t buf[5])
{
    uint8_t ant_id = buf[0];
    uint16_t rssi_val = (uint16_t)(((uint16_t)buf[2] << 8) | buf[1]);
    uint8_t ant_idx = (uint8_t)(ant_id - 0xA1u);
    uint8_t ant_array_idx = (uint8_t)(ant_idx - 1u);
    uint32_t now_ms = TIMER_GetMillis();

    if (PKES_Core_IsDuplicateRssi(ant_array_idx, rssi_val, now_ms) != 0u)
    {
        return;
    }

    PKES_Core_StoreRssiSample(ant_array_idx, rssi_val, now_ms);

    if (ant_id == 0xA2u)
    {
        PKES_Core_StartLedPulse(0x01u);
    }
    if (ant_id == 0xA3u)
    {
        PKES_Core_StartLedPulse(0x02u);
    }
    if (ant_id == 0xA4u)
    {
        PKES_Core_StartLedPulse(0x04u);
    }
    if (ant_id == 0xA5u)
    {
        PKES_Core_StartLedPulse(0x05u);
    }

    CAN_App_SendKeyRssi((uint8_t)PKES_CORE_KEY_ID,
                        rssi_val,
                        ant_idx);

    CAN_App_SendSysState(PKES_SYS_RF_RECEIVED,
                         PKES_STATUS_FRAME_VALID,
                         PKES_REGION_UNKNOWN,
                         ant_idx,
                         PKES_LOCK_KEEP,
                         PKES_TRIGGER_NONE,
                         (uint8_t)(PKES_FLAG_CRC_OK | PKES_FLAG_RSSI_VALID));
}

void PKES_Core_Init(void)
{
    s_ant_poll_idx = 0u;
    s_led1_ticks = 0u;
    s_led2_ticks = 0u;
    s_led3_ticks = 0u;
    s_idle_lf_ticks = 0u;
    PKES_Core_ResetRssiBuffers();
    active_antenna = s_antenna_list[s_ant_poll_idx];

    PKES_Core_SendStandbyState();
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

void PKES_Core_ProcessRFData(const uint8_t buf[5])
{
    uint16_t crc_calc = PKES_Core_Crc16CcittFalse(buf, 3u);
    uint16_t crc_rx = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);

    s_idle_lf_ticks = 0u;

    if (crc_calc != crc_rx)
    {
        return;
    }

    if (buf[0] == 0x84u)
    {
        PKES_Core_HandleActiveKeyFrame(buf);
    }
    else if ((buf[0] >= 0xA2u) && (buf[0] <= 0xA5u))
    {
        PKES_Core_HandlePassiveRssiFrame(buf);
    }
}
