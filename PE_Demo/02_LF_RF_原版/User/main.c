/* ===========================================  Includes  =========================================== */
#include "ac7840x.h"
#include "debugout_ac7840x.h"
#include "ckgen_hw.h"
#include "clock_config.h"
#include "spi.h"
#include "gpio.h"
#include "LF.h"
#include "RF.h"
#include "fortissimo.h"
#include "timer.h"
#include "timer_drv.h"
#include "timer_hw.h"
#include <stdint.h>

/* ============================================  Define  ============================================ */
#define LF_TIMER_START() TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_0)
#define LF_TIMER_STOP() TIMER_DRV_StopChannels(0, 1 << TIMER_CHANNEL_0)
#define ANTENNA_COUNT 4u

/* ==========================================  Variables  =========================================== */
extern void SendLFWakeUp(void);
extern uint8_t active_antenna;
extern volatile uint8_t gRFFull;
extern volatile uint8_t gRf_Buf[25];

static const uint8_t antenna_list[ANTENNA_COUNT] = {1, 2, 3, 4};
static uint8_t ant_poll_idx = 0u;
static uint8_t rf_data[3];
static uint8_t buf[5];
static uint8_t got_data;

/* ==========================================  Functions  =========================================== */
void SystemClock_Config(void)
{
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
}

static uint16_t crc16_ccitt_false(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;
    for (size_t i = 0; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; b++)
        {
            if (crc & 0x8000u)
                crc = (crc << 1) ^ 0x1021u;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

/* ============================================  main  ============================================== */
int main(void)
{
    SystemClock_Config();
    InitDebug();
    SPI_Master_Init();
    GPIO_Init();
    LF_TIMER_Init();
    LF_TIMER_STOP();
		RF_TIMER_Init();
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 1);

    active_antenna = antenna_list[ant_poll_idx];
    LF_TIMER_START();

    while (1)
    {
        /* ── 1. LF 轮询 ──────────────────────────────────────────────── */
        if (LF_timerOutFlag)
        {
            LF_timerOutFlag = 0;
            SendLFWakeUp();
            ant_poll_idx = (uint8_t)((ant_poll_idx + 1u) % ANTENNA_COUNT);
            active_antenna = antenna_list[ant_poll_idx];
        }

        /* ── 2. RF 数据接收与CRC校验 ─────────────────────────────────── */

				if (gRFFull) 
				{
						for (int i = 0; i < 5; i++) 
						{
								buf[i] = gRf_Buf[i];
						}
						gRFFull = 0;
						got_data = 1;
				}


         if (got_data)
				{
						uint16_t crc_calc = crc16_ccitt_false(buf, 3);
						uint16_t crc_rx   = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);

						if (crc_calc == crc_rx)
						{
								int8_t idx = buf[0];

								
								if (buf[0] == 0xA2)
								{
										LED1_ON;    
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										LED1_OFF;    
								}
								if (buf[0] == 0xA3)
								{
										LED2_ON;    
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										LED2_OFF;    
								}if (buf[0] == 0xA4)
								{
										LED3_ON;    
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										LED3_OFF;    
								}if (buf[0] == 0xA5)
								{
										LED1_ON;  
										LED3_ON;					
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										LED1_OFF; 
										LED3_OFF;
								}
								if (buf[1] == 0x63)
								{
										LED1_ON;  
										LED2_ON;
										LED3_ON;					
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										LED1_OFF; 
										LED3_OFF;
										LED2_OFF;
								}
								if (buf[2] == 0x62)
								{
										//LED1_ON;  
										LED2_ON;
										LED3_ON;					
										for (volatile uint32_t i = 0; i < 500000; i++) { } 
										//LED1_OFF; 
										LED3_OFF;
										LED2_OFF;
								}
						}
						
				}
     }
}
/* =============================================  EOF  ============================================== */
