/* ===========================================  Includes  =========================================== */
#include "ac7840x.h"
#include "debugout_ac7840x.h"
#include "ckgen_hw.h"
#include "clock_config.h"
#include "spi.h"
#include "gpio.h"
#include "LF.h"
#include "RF.h"
#include "Fortissimo.h"
#include "pkes_can_app.h"
#include "pkes_can_protocol.h"
#include "pkes_core.h"
#include "pkes_lock_app.h"
#include "timer.h"
#include "timer_drv.h"
#include "timer_hw.h"
#include <stdint.h>

/* ============================================  Define  ============================================ */
#define LF_TIMER_START() TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_0)
#define LF_TIMER_STOP() TIMER_DRV_StopChannels(0, 1 << TIMER_CHANNEL_0)
#define ANTENNA_COUNT 4u
#define HANDLE_LF_POLL_ROUNDS 1u
#define HANDLE_LF_POLL_TICKS (ANTENNA_COUNT * HANDLE_LF_POLL_ROUNDS)
#define APP_CORE_TICK_MS 300u
#define DH_EVENT_LEFT_DRIVER 0x01u
#define DH_EVENT_RIGHT_PASSENGER 0x04u

typedef struct
{
    uint8_t event_mask;
    uint8_t region_code;
} dh_handle_position_t;

/* ==========================================  Variables  =========================================== */
extern uint8_t active_antenna;
extern volatile uint8_t gRFFull;
extern volatile uint8_t gRf_Buf[25];

static const uint8_t antenna_list[ANTENNA_COUNT] = {1u, 2u, 3u, 4u};
static uint8_t ant_poll_idx;
static uint8_t buf[5];
static uint8_t got_data;
static uint8_t lf_poll_active;
static uint8_t lf_poll_ticks;
static uint8_t lf_poll_stop_pending;
static uint32_t app_tick_last_ms;

static const dh_handle_position_t handle_position_map[] =
{
    {DH_EVENT_LEFT_DRIVER, PKES_REGION_LEFT_DOOR},
    {DH_EVENT_RIGHT_PASSENGER, PKES_REGION_RIGHT_DOOR}
};

static void DelayLoop(volatile uint32_t count)
{
    while (count--)
    {
    }
}

static void ATA5293_Enable(void)
{
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 1);
    DelayLoop(2000000u);
}

static void ATA5293_Disable(void)
{
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 0);
    DelayLoop(200000u);
}

/* ==========================================  Functions  =========================================== */
void SystemClock_Config(void)
{
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
}

static void DH_Init(void)
{
    lf_ata5293_SWRI(DHFCTL, 0x01);
    lf_ata5293_SWRI(DHEN, 0x45);
    lf_ata5293_SWRI(DHCT, DH_DCT_5_22mA);
    lf_ata5293_SWRI(DHOC, DH_END_0_25ms | DHOCL0_60mA | DHOCL1_60mA);
    lf_ata5293_SWRI(DHGRPS, 0x00);
    lf_ata5293_SWRI(DHEM, 0x05);
    lf_ata5293_SWRI(DHEDG, 0x05);
    lf_ata5293_SWRI(SQECM1, 0x05);
    lf_ata5293_SWRI(STRM, 0x20);

    lf_ata5293_HALT();
    lf_ata5293_GSLP();
    lf_ata5293_WAITC(1, 3);
    lf_ata5293_GID();
    lf_ata5293_WAIT(0, 50);
    lf_ata5293_LD(R1, DHFAULT);
    lf_ata5293_SKIF(1);
    lf_ata5293_JMPL(0);
    lf_ata5293_NOP();
    lf_ata5293_SLBL(0, 0);
    lf_ata5293_GID();
    lf_ata5293_RUN();
}

static uint8_t DH_GetEvent(void)
{
    uint8_t dhes = 0u;

    lf_ata5293_SRD(DHES, &dhes);
    return (uint8_t)(dhes & 0x05u);
}

static void DH_ClearEvent(void)
{
    lf_ata5293_SWRI(DHFCTL, 0x01);
}

static uint8_t DH_GetHandleRegion(uint8_t event)
{
    uint8_t i;

    for (i = 0u; i < (uint8_t)(sizeof(handle_position_map) / sizeof(handle_position_map[0])); i++)
    {
        if ((event & handle_position_map[i].event_mask) != 0u)
        {
            return handle_position_map[i].region_code;
        }
    }

    return PKES_REGION_UNKNOWN;
}

static void DH_StartLFWindow(uint8_t region_code)
{
    LF_TIMER_STOP();
    ATA5293_Disable();
    ATA5293_Enable();

    lf_poll_active = 1u;
    lf_poll_ticks = HANDLE_LF_POLL_TICKS;
    lf_poll_stop_pending = 0u;
    ant_poll_idx = 0u;
    active_antenna = antenna_list[ant_poll_idx];
    LF_timerOutFlag = 1u;
    LF_TIMER_START();
}

static void DH_StopLFWindow(void)
{
    LF_TIMER_STOP();
    LF_timerOutFlag = 0u;
    lf_poll_active = 0u;
    lf_poll_ticks = 0u;
    lf_poll_stop_pending = 0u;
    LED1_OFF;
    LED2_OFF;
    Lock_App_RefreshLedState();
    PKES_Core_FinalizeHandleRanging(TIMER_GetMillis());
    PKES_Core_EndHandleRanging();

    ATA5293_Disable();
    ATA5293_Enable();
    DH_Init();
    DH_ClearEvent();
    lf_ata5293_RFS();
    (void)checkStatus();
}

/* ============================================  main  ============================================== */
int main(void)
{
    SystemClock_Config();
    InitDebug();
    SPI_Master_Init();
    GPIO_Init();
    CAN_App_Init();
    Lock_App_Init();
    PKES_Core_Init();
    LF_TIMER_Init();
    LF_TIMER_STOP();
    RF_TIMER_Init();
    app_tick_last_ms = TIMER_GetMillis();
    ATA5293_Enable();
    DH_Init();
    DH_ClearEvent();
    lf_ata5293_RFS();
    (void)checkStatus();

    ant_poll_idx = 0u;
    active_antenna = antenna_list[ant_poll_idx];
    lf_poll_active = 0u;
    lf_poll_ticks = 0u;
    lf_poll_stop_pending = 0u;
    LED1_OFF;
    LED2_OFF;

    while (1)
    {
        uint32_t now_ms;

        CAN_App_Task();

        now_ms = TIMER_GetMillis();
        if ((uint32_t)(now_ms - app_tick_last_ms) >= APP_CORE_TICK_MS)
        {
            app_tick_last_ms = now_ms;
            PKES_Core_UpdateTick();
        }

        if (lf_poll_active == 0u)
        {
            if ((GPIO_DRV_ReadPins(IRQ_GPIO) & (1u << IRQ_PIN)) != 0u)
            {
                uint8_t event = DH_GetEvent();

                if (event != 0u)
                {
                    uint8_t region_code = DH_GetHandleRegion(event);

                    DH_ClearEvent();

                    if ((event & DH_EVENT_LEFT_DRIVER) != 0u)
                    {
                        LED1_ON;
                    }
                    if ((event & DH_EVENT_RIGHT_PASSENGER) != 0u)
                    {
                        LED2_ON;
                    }
                    if (region_code != PKES_REGION_UNKNOWN)
                    {
                        CAN_App_SendSysState(PKES_SYS_TRIGGER_DETECTED,
                                             PKES_STATUS_NONE,
                                             PKES_REGION_UNKNOWN,
                                             0u,
                                             PKES_Core_GetCanLockState(),
                                             PKES_TRIGGER_HANDLE,
                                             0u);
                        PKES_Core_StartHandleRanging(region_code);
                        DH_StartLFWindow(region_code);
                    }
                }
            }
        }

        /* 门把手触发后，LF 定时器触发一次发一根天线，每根天线轮询 1 次。 */
        if (LF_timerOutFlag)
        {
            LF_timerOutFlag = 0u;

            if (lf_poll_active != 0u)
            {
                if (lf_poll_stop_pending != 0u)
                {
                    lf_poll_stop_pending = 2u;
                }
                else
                {
                    SendLFWakeUp();
                    CAN_App_SendSysState(PKES_SYS_LF_WAKEUP,
                                         PKES_STATUS_LF_WAKEUP_OK,
                                         PKES_REGION_UNKNOWN,
                                         active_antenna,
                                         PKES_Core_GetCanLockState(),
                                         PKES_TRIGGER_HANDLE,
                                         0u);
                    CAN_App_SendSysState(PKES_SYS_WAIT_RF,
                                         PKES_STATUS_NONE,
                                         PKES_REGION_UNKNOWN,
                                         active_antenna,
                                         PKES_Core_GetCanLockState(),
                                         PKES_TRIGGER_HANDLE,
                                         0u);

                    ant_poll_idx = (uint8_t)((ant_poll_idx + 1u) % ANTENNA_COUNT);
                    active_antenna = antenna_list[ant_poll_idx];

                    if (lf_poll_ticks > 0u)
                    {
                        lf_poll_ticks--;
                    }
                    if (lf_poll_ticks == 0u)
                    {
                        lf_poll_stop_pending = 1u;
                    }
                }
            }
        }

        /* RF 收包：保留原版 5 字节复制路径，解析与 CAN/继电器交给 pkes_core。 */
        if (gRFFull != 0u)
        {
            DisableInterrupts;
            if (gRFFull != 0u)
            {
                uint8_t i;

                for (i = 0u; i < 5u; i++)
                {
                    buf[i] = gRf_Buf[i];
                }
                gRFFull = 0u;
                got_data = 1u;
            }
            EnableInterrupts;
        }

        if (got_data != 0u)
        {
            PKES_Core_ProcessRFData(buf);
            got_data = 0u;
        }

        if (lf_poll_stop_pending == 2u)
        {
            DH_StopLFWindow();
        }
    }
}
/* =============================================  EOF  ============================================== */
