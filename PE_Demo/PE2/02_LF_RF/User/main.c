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

/* ==========================================  Variables  =========================================== */
extern uint8_t active_antenna;
extern volatile uint8_t gRFFull;
extern volatile uint8_t gRf_Buf[25];

static const uint8_t antenna_list[ANTENNA_COUNT] = {1u, 2u, 3u, 4u};
static uint8_t ant_poll_idx;
static uint8_t buf[5];
static uint8_t got_data;

/* ==========================================  Functions  =========================================== */
void SystemClock_Config(void)
{
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
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
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 1);

    ant_poll_idx = 0u;
    active_antenna = antenna_list[ant_poll_idx];
    LF_TIMER_START();

    while (1)
    {
        CAN_App_Task();

        /* 原版 LF 轮询路径：定时发送当前天线 LF，然后切到下一根天线。 */
        if (LF_timerOutFlag)
        {
            LF_timerOutFlag = 0u;

            SendLFWakeUp();
            CAN_App_SendSysState(PKES_SYS_LF_WAKEUP,
                                 PKES_STATUS_LF_WAKEUP_OK,
                                 PKES_REGION_UNKNOWN,
                                 active_antenna,
                                 PKES_LOCK_KEEP,
                                 PKES_TRIGGER_NONE,
                                 0u);

            ant_poll_idx = (uint8_t)((ant_poll_idx + 1u) % ANTENNA_COUNT);
            active_antenna = antenna_list[ant_poll_idx];

            PKES_Core_UpdateTick();
        }

        /* RF 收包：保留原版 5 字节复制路径，解析与 CAN/继电器交给 pkes_core。 */
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

        if (got_data != 0u)
        {
            PKES_Core_ProcessRFData(buf);
            got_data = 0u;
        }
    }
}
/* =============================================  EOF  ============================================== */
