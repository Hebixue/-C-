#include "ac7840x.h"
#include "debugout_ac7840x.h"
#include "ckgen_hw.h"
#include "ckgen_drv.h"
#include "clock_config.h"
#include "port_hw.h"
#include "gpio_hw.h"
#include "spi_master_drv.h"
#include "spi.h"
#include "gpio.h"
#include "fortissimo.h"
#include "timer.h"
#include "timer_drv.h"

/* ── IRQ 引脚 ── */
#define IRQ_GPIO  (GPIOB)
#define IRQ_PIN   (28)

/* ── 定时器宏 ── */
#define LF_TIMER_START()  TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_0)
#define LF_TIMER_STOP()   TIMER_DRV_StopChannels(0,  1 << TIMER_CHANNEL_0)

/* ── 参数配置 ── */
#define LED_ON_TICKS    4u    /* 250ms x 4 = 1s */
#define DEBOUNCE_COUNT  20u

/* ── 状态变量 ── */
static uint8_t led1_tick     = 0u;
static uint8_t led2_tick     = 0u;
static uint8_t debounce_cnt = 0u;

static void DelayLoop(volatile uint32_t count)
{
    while (count--)
    {
    }
}

void SystemClock_Config(void)
{
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
}

static void DH_Init(void)
{
    lf_ata5293_SWRI(DHFCTL,  0x01);                        /* 清除DH缓冲 */
    lf_ata5293_SWRI(DHEN,    0x45);                        /* 使能温度检测 + DH2\DH0 */
    lf_ata5293_SWRI(DHCT,    DH_DCT_5_22mA);               /* 电流阈值，按实际R值调整 */
    lf_ata5293_SWRI(DHOC,    DH_END_0_25ms | DHOCL0_60mA | DHOCL1_60mA); /* DH0/DH2 过流保护 */
    lf_ata5293_SWRI(DHGRPS,  0x00);                        /* DH2\0归入group0 */
    lf_ata5293_SWRI(DHEM,    0x05);                        /* 使能DH2\0事件掩码 */
    lf_ata5293_SWRI(DHEDG,   0x05);                        /* DH0/DH2 使用相同触发边沿 */
    lf_ata5293_SWRI(SQECM1,  0x05);                        /* DH2\0作为触发条件 */
    lf_ata5293_SWRI(STRM,    0x20);                        /* DH事件触发IRQ */

    /* FTS 序列 */
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
    uint8_t dhes = 0;
    lf_ata5293_SRD(DHES, &dhes);
    return (uint8_t)(dhes & 0x05);
}

static void DH_ClearEvent(void)
{
    lf_ata5293_SWRI(DHFCTL, 0x01);
}

static void LED_SelfTest(void)
{
    LED1_ON;
    DelayLoop(800000u);
    LED1_OFF;
    DelayLoop(300000u);

    LED2_ON;
    DelayLoop(800000u);
    LED2_OFF;
    DelayLoop(300000u);

    LED3_ON;
    DelayLoop(800000u);
    LED3_OFF;
}

int main(void)
{
    SystemClock_Config();
    InitDebug();
    SPI_Master_Init();
    GPIO_Init();

    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 1);

    for (volatile uint32_t i = 0; i < 2000000; i++);

    DH_Init();
    DH_ClearEvent();
		lf_ata5293_RFS();        /* 清除故障状态寄存器 */
		checkStatus();
    LF_TIMER_Init();
    LF_TIMER_START();

    LED1_OFF;
		LED2_OFF;
    LED3_OFF;
    LED_SelfTest();

    while (1)
    {
        /* ══ 1. IRQ 检测 + 消抖 ══ */
        if ((GPIO_DRV_ReadPins(IRQ_GPIO) & (1u << IRQ_PIN)) != 0u)
        {
            debounce_cnt++;
             uint8_t event = DH_GetEvent();
                if (event != 0u)
                {
                    DH_ClearEvent();

                    if (event & 0x01)   /* DH0 触发 -> LED1 */
                    {
                        LED1_ON;
                        led1_tick = 0u;
                    }
                    if (event & 0x04)   /* DH2 触发 -> LED2 */
                    {
                        LED2_ON;
                        led2_tick = 0u;
                    }
                }
                debounce_cnt = 0u;
        }
        else
        {
            debounce_cnt = 0u;
        }

        /* ══ 2. LED 计时熄灭 ══ */
        if (LF_timerOutFlag)
        {
            LF_timerOutFlag = 0;

            /* LED1 倒计时 */
            if (led1_tick < LED_ON_TICKS)
            {
                led1_tick++;
                if (led1_tick >= LED_ON_TICKS)
                {
                    LED1_OFF;
                }
            }

            /* LED2 倒计时 */
            if (led2_tick < LED_ON_TICKS)
            {
                led2_tick++;
                if (led2_tick >= LED_ON_TICKS)
                {
                    LED2_OFF;
                }
            }
        }
    }
}
