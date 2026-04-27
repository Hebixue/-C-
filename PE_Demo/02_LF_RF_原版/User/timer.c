

/* ===========================================  Includes  =========================================== */
#include "ac7840x.h"
#include "string.h"
#include "timer_drv.h"
#include "timer_hw.h"
#include "RF.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
uint32_t LF_timerOutFlag = 0; /* 定时标志位 */
/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */

/*!* @brief TIMER0中断回调函数
 *
 * @param[in] *device wpara lpara
 * @return none
 */
void TIMER1_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    LD_RF_Recive();
}

/*!
 * @brief TIMER初始化函数
 *
 * @param[in] none
 * @return none
 *注意：timer时钟源在clock_config.c文件的peripheralClockConfig0结构体中第二个元素配置
 */
void RF_TIMER_Init(void)
{
    timer_user_channel_config_t timerConfig1;
    memset(&timerConfig1, 0U, sizeof(timerConfig1));

    /*TIMER参数配置*/
    timerConfig1.timerMode = TIMER_PERIODIC_COUNTER;            /* 32bit计数 */
    timerConfig1.periodUnits = TIMER_PERIOD_UNITS_MICROSECONDS; /* 以us为单位计数 */
    timerConfig1.period = 25U;                                  /* 25μs为周期 */
    timerConfig1.triggerSource = TIMER_TRIGGER_SOURCE_INTERNAL; /* 选择内部触发 */
    timerConfig1.triggerSelect = 0x00U;                         /* 选择通道0作为触发源，触发是否奏效由TSOT决定，1：触发源上升沿来时开始计数；0：使能后就开始计数，触发源无效 */
    timerConfig1.enableReloadOnTrigger = false;                 /* 触发源来时不重新加载计数值 */
    timerConfig1.enableStopOnInterrupt = false;                 /* 发生一次中断后不停止Timer的计数 */
    timerConfig1.enableStartOnTrigger = false;                  /* 配置ENRx=1即会使Timer开始计数,该位决定触发源是否奏效 */
    timerConfig1.chainChannel = false;                          /* 禁能链接 */
    timerConfig1.isInterruptEnabled = true;                     /* 使能中断 */
    timerConfig1.callback = TIMER1_Callback;                    /* 回调函数配置 */

    /*TIMER调用底层接口*/
    TIMER_DRV_Init(0, true);
    TIMER_DRV_InitChannel(0, TIMER_CHANNEL_1, &timerConfig1);
    TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_1);
}

/*!* @brief TIMER0中断回调函数
 *
 * @param[in] *device wpara lpara
 * @return none
 */
void TIMER0_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    LF_timerOutFlag = 1; /*TIMER定时标志位*/
}

/*!
 * @brief TIMER初始化函数
 *
 * @param[in] none
 * @return none
 *注意：timer时钟源在clock_config.c文件的peripheralClockConfig0结构体中第二个元素配置
 */
void LF_TIMER_Init(void)
{
    timer_user_channel_config_t timerConfig;
    memset(&timerConfig, 0U, sizeof(timerConfig));

    /*TIMER参数配置*/
    timerConfig.timerMode = TIMER_PERIODIC_COUNTER;            /* 32bit计数 */
    timerConfig.periodUnits = TIMER_PERIOD_UNITS_MICROSECONDS; /* 以us为单位计数 */
    timerConfig.period = 1000000U;                              /* 1s为周期 */
    timerConfig.triggerSource = TIMER_TRIGGER_SOURCE_INTERNAL; /* 选择内部触发 */
    timerConfig.triggerSelect = 0x00U;                         /* 选择通道0作为触发源，触发是否奏效由TSOT决定，1：触发源上升沿来时开始计数；0：使能后就开始计数，触发源无效 */
    timerConfig.enableReloadOnTrigger = false;                 /* 触发源来时不重新加载计数值 */
    timerConfig.enableStopOnInterrupt = false;                 /* 发生一次中断后不停止Timer的计数 */
    timerConfig.enableStartOnTrigger = false;                  /* 配置ENRx=1即会使Timer开始计数,该位决定触发源是否奏效 */
    timerConfig.chainChannel = false;                          /* 禁能链接 */
    timerConfig.isInterruptEnabled = true;                     /* 使能中断 */
    timerConfig.callback = TIMER0_Callback;                    /* 回调函数配置 */

    /*TIMER调用底层接口*/
    TIMER_DRV_Init(0, true);
    TIMER_DRV_InitChannel(0, TIMER_CHANNEL_0, &timerConfig);
    TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_0);
}
