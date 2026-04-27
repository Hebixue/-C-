/* ===========================================  Includes  =========================================== */
#include "ac7840x.h"
#include "string.h"
#include "timer_drv.h"
#include "timer_hw.h"
#include "RF.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
uint32_t LF_timerOutFlag = 0;   /* 低频(LF)定时器超时标志位 */
volatile uint32_t g_system_ms = 0u; /* 系统毫秒级时间戳 */
static uint8_t s_ms_divider = 0u;   /* 毫秒分频计数器，用于将微秒中断转换为毫秒 */
/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */

/*!
 * @brief TIMER1 中断回调函数 (原乱码写为TIMER0，此处根据函数名修正为TIMER1)
 *
 * @param[in] device 设备指针
 * @param[in] wpara  字参数
 * @param[in] lpara  长字参数
 * @return none
 */
void TIMER1_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    LD_RF_Recive(); /* 调用射频接收函数 */

    /* 25us 进一次中断，累计 40 次即为 1000us (1ms) */
    s_ms_divider++;
    if (s_ms_divider >= 40u)
    {
        s_ms_divider = 0u;
        g_system_ms++; /* 系统运行毫秒数累加 */
    }
}

/*!
 * @brief 射频(RF) TIMER 初始化函数
 *
 * @param[in] none
 * @return none
 * 注意：timer 时钟源在 clock_config.c 文件的 peripheralClockConfig0 结构体中的第二个元素里配置
 */
void RF_TIMER_Init(void)
{
    timer_user_channel_config_t timerConfig1;
    memset(&timerConfig1, 0U, sizeof(timerConfig1));

    /* TIMER 配置结构体参数设置 */
    timerConfig1.timerMode = TIMER_PERIODIC_COUNTER;            /* 32bit 周期计数模式 */
    timerConfig1.periodUnits = TIMER_PERIOD_UNITS_MICROSECONDS; /* 计数周期单位设为微秒(us) */
    timerConfig1.period = 25U;                                  /* 设定周期为 25us */
    timerConfig1.triggerSource = TIMER_TRIGGER_SOURCE_INTERNAL; /* 选择内部触发源 */
    
    /* 触发源选择：选择通道0作为触发源。
       是否有效取决于 TSOT 位：填1表示触发源到来时开始计数；填0表示使能后直接开始计数，触发源无效。*/
    timerConfig1.triggerSelect = 0x00U;                         
    timerConfig1.enableReloadOnTrigger = false;                 /* 触发源到来时不重新加载计数值 */
    timerConfig1.enableStopOnInterrupt = false;                 /* 产生一次中断后不停止 Timer 计数（连续运行） */
    timerConfig1.enableStartOnTrigger = false;                  /* 不通过触发源启动计数（配置为使能后立即运行） */
    timerConfig1.chainChannel = false;                          /* 不开启通道级联功能 */
    timerConfig1.isInterruptEnabled = true;                     /* 使能定时器中断 */
    timerConfig1.callback = TIMER1_Callback;                    /* 注册中断回调函数 */

    /* TIMER 底层驱动接口调用与初始化 */
    TIMER_DRV_Init(0, true);
    TIMER_DRV_InitChannel(0, TIMER_CHANNEL_1, &timerConfig1);
    TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_1);
}

/*!
 * @brief TIMER0 中断回调函数
 *
 * @param[in] device 设备指针
 * @param[in] wpara  字参数
 * @param[in] lpara  长字参数
 * @return none
 */
void TIMER0_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    LF_timerOutFlag = 1; /* 置位 TIMER0 超时标志位，供主循环轮询处理 */
}

/*!
 * @brief 获取系统毫秒数
 *
 * @return uint32_t 返回系统启动以来的毫秒数
 */
uint32_t TIMER_GetMillis(void)
{
    return g_system_ms;
}

/*!
 * @brief 低频(LF) TIMER 初始化函数
 *
 * @param[in] none
 * @return none
 * 注意：timer 时钟源在 clock_config.c 文件的 peripheralClockConfig0 结构体中的第二个元素里配置
 */
void LF_TIMER_Init(void)
{
    timer_user_channel_config_t timerConfig;
    memset(&timerConfig, 0U, sizeof(timerConfig));

    /* TIMER 配置结构体参数设置 */
    timerConfig.timerMode = TIMER_PERIODIC_COUNTER;            /* 32bit 周期计数模式 */
    timerConfig.periodUnits = TIMER_PERIOD_UNITS_MICROSECONDS; /* 计数周期单位设为微秒(us) */
    timerConfig.period = 85000U;                               /* 设定周期为 85000us，即 85ms LF 轮询节奏 */
    timerConfig.triggerSource = TIMER_TRIGGER_SOURCE_INTERNAL; /* 选择内部触发源 */
    
    /* 触发源选择：选择通道0作为触发源。
       是否有效取决于 TSOT 位：填1表示触发源到来时开始计数；填0表示使能后直接开始计数，触发源无效。*/
    timerConfig.triggerSelect = 0x00U;                         
    timerConfig.enableReloadOnTrigger = false;                 /* 触发源到来时不重新加载计数值 */
    timerConfig.enableStopOnInterrupt = false;                 /* 产生一次中断后不停止 Timer 计数（连续运行） */
    timerConfig.enableStartOnTrigger = false;                  /* 不通过触发源启动计数（配置为使能后立即运行） */
    timerConfig.chainChannel = false;                          /* 不开启通道级联功能 */
    timerConfig.isInterruptEnabled = true;                     /* 使能定时器中断 */
    timerConfig.callback = TIMER0_Callback;                    /* 注册中断回调函数 */

    /* TIMER 底层驱动接口调用与初始化 */
    TIMER_DRV_Init(0, true);
    TIMER_DRV_InitChannel(0, TIMER_CHANNEL_0, &timerConfig);
    TIMER_DRV_StartChannels(0, 1 << TIMER_CHANNEL_0);
}

