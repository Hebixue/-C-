#include "pkes_can_app.h"

#include "can.h"
#include "pkes_can_protocol.h"
#include "timer.h"

#define CAN_APP_REPEAT_COUNT 1u
#define CAN_APP_REPEAT_DELAY_MS 100u
#define CAN_APP_QUEUE_SIZE 64u

typedef struct
{
    uint32_t id;
    uint8_t data[PKES_CAN_DLC];
} can_app_frame_t;

static uint8_t s_can_seq;
static can_app_frame_t s_queue[CAN_APP_QUEUE_SIZE];
static uint8_t s_queue_head;
static uint8_t s_queue_tail;
static uint8_t s_queue_count;
static can_app_frame_t s_active_frame;
static uint8_t s_active_valid;
static uint8_t s_active_repeat_left;
static uint32_t s_next_send_ms;

static void CAN_App_CopyData(uint8_t dst[PKES_CAN_DLC], const uint8_t src[PKES_CAN_DLC])
{
    uint8_t i;

    for (i = 0u; i < PKES_CAN_DLC; i++)
    {
        dst[i] = src[i];
    }
}

static void CAN_App_Enqueue(uint32_t id, const uint8_t data[PKES_CAN_DLC])
{
    if (s_queue_count >= CAN_APP_QUEUE_SIZE)
    {
        return;
    }

    s_queue[s_queue_tail].id = id;
    CAN_App_CopyData(s_queue[s_queue_tail].data, data);
    s_queue_tail = (uint8_t)((s_queue_tail + 1u) % CAN_APP_QUEUE_SIZE);
    s_queue_count++;
}

void CAN_App_Init(void)
{
    s_can_seq = 0u;
    s_queue_head = 0u;
    s_queue_tail = 0u;
    s_queue_count = 0u;
    s_active_valid = 0u;
    s_active_repeat_left = 0u;
    s_next_send_ms = 0u;
    CAN_Init(0);
}

void CAN_App_Task(void)
{
    uint32_t now_ms = TIMER_GetMillis();

    if ((s_active_valid == 0u) && (s_queue_count > 0u))
    {
        s_active_frame.id = s_queue[s_queue_head].id;
        CAN_App_CopyData(s_active_frame.data, s_queue[s_queue_head].data);
        s_queue_head = (uint8_t)((s_queue_head + 1u) % CAN_APP_QUEUE_SIZE);
        s_queue_count--;

        s_active_valid = 1u;
        s_active_repeat_left = CAN_APP_REPEAT_COUNT;
        s_next_send_ms = now_ms;
    }

    if (s_active_valid == 0u)
    {
        return;
    }

    if ((int32_t)(now_ms - s_next_send_ms) < 0)
    {
        return;
    }

    if (CAN_SendData(0, s_active_frame.id, s_active_frame.data, PKES_CAN_DLC) != STATUS_SUCCESS)
    {
        return;
    }

    s_active_repeat_left--;

    if (s_active_repeat_left == 0u)
    {
        s_active_valid = 0u;
    }
    else
    {
        s_next_send_ms = now_ms + CAN_APP_REPEAT_DELAY_MS;
    }
}

void CAN_App_SendSysState(uint8_t sys_state,
                          uint8_t status_code,
                          uint8_t region_code,
                          uint8_t antenna_id,
                          uint8_t lock_state,
                          uint8_t trigger_src,
                          uint8_t flags)
{
    uint8_t data[PKES_CAN_DLC];
    pkes_can_sys_status_t frame;

    frame.seq = s_can_seq++;
    frame.sys_state = sys_state;
    frame.status_code = status_code;
    frame.region_code = region_code;
    frame.antenna_id = antenna_id;
    frame.lock_state = lock_state;
    frame.trigger_src = trigger_src;
    frame.flags = flags;

    PKES_CAN_PackSysStatus(&frame, data);
    CAN_App_Enqueue(PKES_CAN_ID_SYS_STATUS, data);
}

void CAN_App_SendKeyRssi(uint8_t key_id,
                         uint16_t rssi,
                         uint8_t rssi_ant,
                         uint8_t cmd)
{
    uint8_t data[PKES_CAN_DLC];
    pkes_can_key_rssi_t frame;

    frame.key_id = key_id;
    frame.rssi_l = (uint8_t)(rssi & 0xFFu);
    frame.rssi_h = (uint8_t)((rssi >> 8) & 0xFFu);
    frame.rssi_ant = rssi_ant;
    frame.cmd = cmd;
    frame.reserved1 = 0u;
    frame.reserved2 = 0u;
    frame.reserved3 = 0u;

    PKES_CAN_PackKeyRssi(&frame, data);
    CAN_App_Enqueue(PKES_CAN_ID_KEY_RSSI, data);
}

void CAN_App_SendDistance(uint8_t antenna_id,
                          uint16_t dist_cm,
                          uint8_t region_code)
{
    uint8_t data[PKES_CAN_DLC];
    pkes_can_distance_t frame;

    frame.antenna_id = antenna_id;
    PKES_CAN_SetDistanceCm(&frame, dist_cm);
    frame.region_code = region_code;
    frame.reserved0 = 0u;
    frame.reserved1 = 0u;
    frame.reserved2 = 0u;
    frame.reserved3 = 0u;

    PKES_CAN_PackDistance(&frame, data);
    CAN_App_Enqueue(PKES_CAN_ID_DISTANCE, data);
}
