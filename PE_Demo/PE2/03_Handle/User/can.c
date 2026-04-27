
#include "debugout_ac7840x.h"
#include "gpio_drv.h"
#include "can_drv.h"
#include "clock_config.h"
#include "gpio.h"

/* ============================================  Define  ============================================ */
#define CAN_SOURCE_CLOCK (60UL)     /* 时钟源选择60M */
#define CAN_MSG_HANDLE_DIRECT (1UL) /* 是否处理消息， 1 是， 0 否 */
#define CAN_TIME_STAMP_ENABLE (0UL) /* 使能/失能CAN时间戳， 1 使能， 0 失能 */

/* ===========================================  Typedef  ============================================ */
typedef struct
{
    uint32_t recvCount; /* 接收消息计数 */
    uint32_t sendCount; /* 发送消息计数*/
} can_cur_node_t;

/* ==========================================  Variables  =========================================== */
/*
 *  波特率和采样点计算公式：
 *  tSeg1 = (S_SEG_1 + 2); tSeg2 = (S_SEG_2 + 1).
 *  BandRate = (SRC_CLK / (S_PRESC + 1) / ((S_SEG_1 + 2) + (S_SEG_2 + 1))), SRC_CLK 为CAN模块时钟源
 *  SamplePoint = (tSeg1 / (tSeg1 + tSeg2)).
 */
#if (CAN_SOURCE_CLOCK == 80UL) /*时钟源为80M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM] =
    {
        {0x1C, 0x09, 0x09, 0x01}, /*   1M, 75% */
        {0x26, 0x09, 0x09, 0x01}, /* 800K, 80% */
        {0x1C, 0x09, 0x09, 0x03}, /* 500K, 75% */
        {0x1C, 0x09, 0x09, 0x07}, /* 250K, 75% */
        {0x1C, 0x09, 0x09, 0x0F}, /* 125K, 75% */
        {0x1C, 0x09, 0x09, 0x13}, /* 100K, 75% */
        {0x1C, 0x09, 0x09, 0x27}, /*  50K, 75% */
};

#elif (CAN_SOURCE_CLOCK == 60UL) /*时钟源为60M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM] =
    {
        {0x0D, 0x04, 0x04, 0x02}, /*   1M, 75% */
        {0x12, 0x04, 0x04, 0x02}, /* 800K, 80% */
        {0x1C, 0x09, 0x09, 0x02}, /* 500K, 75% */
        {0x1C, 0x09, 0x09, 0x05}, /* 250K, 75% */
        {0x1C, 0x09, 0x09, 0x0B}, /* 125K, 75% */
        {0x1C, 0x09, 0x09, 0x0E}, /* 100K, 75% */
        {0x1C, 0x09, 0x09, 0x1D}, /*  50K, 75% */
};

#elif (CAN_SOURCE_CLOCK == 48UL) /* 时钟源为48M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM] =
    {
        {0x22, 0x0B, 0x0B, 0x00}, /*   1M, 75% */
        {0x16, 0x05, 0x05, 0x01}, /* 800K, 80% */
        {0x22, 0x0B, 0x0B, 0x01}, /* 500K, 75%*/
        {0x22, 0x0B, 0x0B, 0x03}, /* 250K, 75% */
        {0x22, 0x0B, 0x0B, 0x07}, /* 125K, 75% */
        {0x22, 0x0B, 0x0B, 0x09}, /* 100K, 75% */
        {0x22, 0x0B, 0x0B, 0x13}, /*  50K, 75% */
};

#elif (CAN_SOURCE_CLOCK == 24UL) /* 时钟源为24M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM] =
    {
        {0x10, 0x05, 0x05, 0x00}, /*   1M, 75% */
        {0x16, 0x05, 0x05, 0x00}, /* 800K, 80% */
        {0x10, 0x05, 0x05, 0x01}, /* 500K, 75% */
        {0x10, 0x05, 0x05, 0x03}, /* 250K, 75% */
        {0x10, 0x05, 0x05, 0x07}, /* 125K, 75% */
        {0x10, 0x05, 0x05, 0x09}, /* 100K, 75% */
        {0x10, 0x05, 0x05, 0x13}, /*  50K, 75% */
};

#elif (CAN_SOURCE_CLOCK == 8UL) /*时钟源为8M*/
static const can_time_segment_t s_canBitrate[CAN_BITRATE_NUM] =
    {
        {0x04, 0x01, 0x01, 0x00}, /*   1M，75% */
        {0x06, 0x01, 0x01, 0x00}, /* 800K, 80% */
        {0x0A, 0x03, 0x03, 0x00}, /* 500K，75% */
        {0x0A, 0x03, 0x03, 0x01}, /* 250K，75% */
        {0x0A, 0x03, 0x03, 0x03}, /* 125K，75% */
        {0x0D, 0x04, 0x03, 0x03}, /* 100K，75% */
        {0x0D, 0x04, 0x03, 0x07}, /*  50K，75% */
};
#endif

static const can_filter_config_t s_canFilterList[16] = /* 过滤器配置*/
    {
        {0x00000001, 0x00000000, CAN_MSG_ID_BOTH}, /* 只接收ID 0x0000_0001和0x001的数据 */
        {0x00000002, 0x00000000, CAN_MSG_ID_STD},  /* 只接收ID 0x002的标准帧数据 */
        {0x00000013, 0x00000000, CAN_MSG_ID_EXT},  /* 只接收ID 0x0000_0013的扩展帧数据 */
        {0x00000124, 0x00000000, CAN_MSG_ID_BOTH}, /* 只接收ID 0x0000_0124和0x124的数据 */
        {0x00000050, 0x1FFFFF0F, CAN_MSG_ID_BOTH}, /* 可接收ID 0xXXXX_XX5X 或 0xX5X的数据，X为任意值 */
        {0x00000060, 0x1FFFFF0F, CAN_MSG_ID_BOTH}, /* 可接收ID 0xXXXX_XX6X 或 0xX6X的数据，X为任意值 */
        {0x00000007, 0x1FFFFFF0, CAN_MSG_ID_BOTH}, /* 可接收ID 0xXXXX_XXX7 或 0xXX7的数据，X为任意值 */
        {0x00000008, 0x1FFFFFF0, CAN_MSG_ID_BOTH}, /* 可接收ID 0xXXXX_XXX8 或 0xXX8的数据，X为任意值 */

        {0x00000009, 0x1FFFFFF0, CAN_MSG_ID_STD},  /* 可接收ID 0xXX9的标准帧数据，X为任意值 */
        {0x0000000a, 0x1FFFFFF0, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXXX_XXXa的扩展帧数据，X为任意值 */
        {0x00000700, 0x1FFFF0FF, CAN_MSG_ID_BOTH}, /* 可接收ID 0xXXXX_X7XX 或 0x7XX的数据，X为任意值 */
        {0x0000c000, 0x1FFF0FFF, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXXX_CXXX 的扩展帧数据，X为任意值 */
        {0x000d0000, 0x1FF0FFFF, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXXX_DXXX 的扩展帧数据，X为任意值 */
        {0x00e00000, 0x1F0FFFFF, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXEX_XXXX 的扩展帧数据，X为任意值 */
        {0x0f000000, 0x10FFFFFF, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXXX_XXXX 的扩展帧数据，X为任意值 */
        {0x10000000, 0x0FFFFFFF, CAN_MSG_ID_EXT},  /* 可接收ID 0xXXXX_XXXX 的扩展帧数据，X为任意值 */
};

static can_cur_node_t s_canCurNode[CAN_INSTANCE_MAX] = {0}; /* 接收信息*/

/*!
 * @brief GPIO 初始化
 *
 * @param[in] instance: CAN 模块通道
 * @return none
 */
void CAN_InitGPIO(uint8_t instance)
{
    if (0U == instance)
    {
        GPIO_DRV_SetMuxModeSel(PORTE, 4U, PORT_MUX_ALT5);  /* CAN0_RX */
        GPIO_DRV_SetMuxModeSel(PORTE, 5U, PORT_MUX_ALT5);  /* CAN0_TX */
        GPIO_DRV_SetMuxModeSel(PORTE, 10U, PORT_MUX_ALT5); /* 配置收发器standby功能*/
    }
    else if (1U == instance)
    {
        GPIO_DRV_SetMuxModeSel(PORTC, 6U, PORT_MUX_ALT3); /* CAN1_RX */
        GPIO_DRV_SetMuxModeSel(PORTC, 7U, PORT_MUX_ALT3); /* CAN1_TX */
        GPIO_DRV_SetMuxModeSel(PORTE, 6U, PORT_MUX_ALT5); /* 配置收发器standby功能*/
    }
    else if (2U == instance)
    {
        GPIO_DRV_SetMuxModeSel(PORTC, 16U, PORT_MUX_ALT3); /* CAN2_RX */
        GPIO_DRV_SetMuxModeSel(PORTC, 17U, PORT_MUX_ALT3); /* CAN2_TX */
        GPIO_DRV_SetMuxModeSel(PORTC, 15U, PORT_MUX_ALT5); /* 配置收发器standby功能 */
    }
    else if (3U == instance)
    {
        GPIO_DRV_SetMuxModeSel(PORTC, 12U, PORT_MUX_ALT5); /* CAN3_RX */
        GPIO_DRV_SetMuxModeSel(PORTC, 13U, PORT_MUX_ALT5); /* CAN3_TX */
        GPIO_DRV_SetMuxModeSel(PORTC, 11U, PORT_MUX_ALT5); /* 配置收发器standby功能 */
    }
}

/*!
 * @brief CAN_GetNormalBitrate 获取波特率
 *
 * @param[in] index: 波特率索引
 * @return 波特率配置
 */
static can_time_segment_t CAN_GetNormalBitrate(can_bitrate_t index)
{
    can_time_segment_t bitrate = {0};

    if (index < CAN_BITRATE_NUM)
    {
        bitrate = s_canBitrate[index];
    }

    return bitrate;
}

/*!
 * @brief CAN_HandleMsg 接收数据处理
 *
 * @param[in] instance: CAN 模块通道
 * @param[in] msgInfo: 接收数据
 * @return none
 */
static void CAN_HandleMsg(uint8_t instance, const can_msg_info_t *msgInfo)
{
    uint8_t i = 0;

    s_canCurNode[instance].recvCount++; /* 节点信息更新 */
#if CAN_MSG_HANDLE_DIRECT               /* 处理数据*/
    if (msgInfo->IDE)
    {
        printf("CAN[%d] RecvCount:%06d ID:%08x DLC:%d", instance, s_canCurNode[instance].recvCount, msgInfo->ID, msgInfo->DLC);
    }
    else
    {
        printf("CAN[%d] RecvCount:%06d  ID:%03x DLC:%d", instance, s_canCurNode[instance].recvCount, msgInfo->ID, msgInfo->DLC);
    }
    if ((0 == msgInfo->RTR) && msgInfo->DLC)
    {
        printf("\r\tData: ");
        for (i = 0; (i < msgInfo->DLC) && (i < 8); i++)
        {
            printf(" %02x", msgInfo->DATA[i]);
        }
    }
    printf("\r\n");
#endif
}

/*!
 * @brief CAN_EventCallback 中断回调函数， 可通过CAN_IRQHandler了解参数具体含义
 *
 * @param[in] instance: CAN 模块通道
 * @param[in] event: 中断事件
 * @param[in] koer: 错误类型
 * @return 0: none
 */
void CAN_EventCallback(uint8_t instance, uint32_t event, uint32_t koer)
{
    can_msg_info_t recvMsg = {0};
    uint8_t recvData[8] = {0};

    if (event & (uint32_t)CAN_EVENT_BUS_ERROR) /* 发生错误后，打印错误消息，仅用于调试 */
    {
        printf("CAN[%d]e: EVEN: %x  KOER: %x\r\n", instance, event, koer);
    }

    if (event & (uint32_t)CAN_EVENT_RECEIVE_DONE) /* 接收数据*/
    {
        while (CAN_DRV_GetRbufStatus(instance))
        {
            recvMsg.DATA = recvData; /* 为接收数据索引中的数据域分配空间， 如果未给接收数据索引的数据域分配空间，直操作会触发Haltfault */
            if (STATUS_SUCCESS == CAN_DRV_Receive(instance, &recvMsg))
            {
                CAN_HandleMsg(instance, &recvMsg);
            }
        }
    }
}

/*!
 * @brief CAN_SendTest 发送数据测试
 *
 * @param[in] instance: CAN 模块通道
 * @param[in] type: CAN 发送缓冲区选择
 *              - CAN_TRANSMIT_PRIMARY
 *              - CAN_TRANSMIT_SECONDARY
 * @return STATUS_SUCCESS: 发送成功   其他值： 发送失败
 */
status_t CAN_SendTest(uint8_t instance, can_transmit_buff_t type)
{
    status_t ret;
    volatile uint32_t timeout;
    static can_msg_info_t s_sendMsg = {0};
    static uint8_t sendData[8] = {0};

    s_sendMsg.ID = 0x220;      /* CAN ID*/
    s_sendMsg.IDE = 0;         /* 标准帧*/
    s_sendMsg.RTR = 0;         /* 非远程帧*/
    s_sendMsg.DLC = 8;         /* 数据长度*/
    s_sendMsg.DATA = sendData; /* 分配空间*/
    s_sendMsg.DATA[0] = 0x11;  /* 填充数据*/
    s_sendMsg.DATA[1] = 0x22;
    s_sendMsg.DATA[2] = 0x33;
    s_sendMsg.DATA[3] = 0x44;
    s_sendMsg.DATA[4] = 0x55;
    s_sendMsg.DATA[5] = 0x66;
    s_sendMsg.DATA[6] = 0x77;
    s_sendMsg.DATA[7] = 0x88;

    do
    {
        ret = CAN_DRV_Send(instance, &s_sendMsg, type);
        timeout++;
    } while ((STATUS_BUSY == ret) && (timeout < 0xFFF));

    if (STATUS_SUCCESS == ret)
    {
        s_canCurNode[instance].sendCount++; /* 节点信息更新 */
    }

    return ret;
}

/*!
 * @brief  CAN_Test, 用于测试发送接收数据
 *
 * @param[in] none
 * @return none
 */
void CAN_Test(void)
{
    uint8_t instance = 0U;
    can_user_config_t canCfg = {0};
    can_bitrate_t bitrateIndex = CAN_BITRATE_500K;     /* 低速波特率设置 */
    can_transmit_buff_t type = CAN_TRANSMIT_SECONDARY; /* 发送缓存设置 */

    canCfg.filterNum = 0U;                  /* 过滤器数量 */
    canCfg.canMode = CAN_NORMAL_MODE;       /* 正常模式 */
    canCfg.interruptEn = true;              /* 使能中断*/
    canCfg.tsMode = CAN_TSMODE_FIFO;        /* 选择FIFO模式*/
    canCfg.tsAmount = CAN_TRANSMIT_SEC_ALL; /* TSALL*/
    canCfg.tpss = false;                    /* 失能PTB单次发送 */
    canCfg.tsss = false;                    /* 失能STB单次发送 */
    canCfg.rom = CAN_ROM_OVER_WRITE;        /* 缓冲区溢出后，新数据会覆盖旧数据*/
    canCfg.errorWarningLimit = 0x0BU;       /* 错误警告限制，当 TECNT|RECTN 超过这个限制，会触发EIF错误中断 */
#if CAN_TIME_STAMP_ENABLE
    canCfg.timeStampEn = true;                           /* 时间戳使能 */
    canCfg.timeStampClkSrc = CAN_TIME_STAMP_CLK_SRC_EXT; /* 时间戳时钟源选择 */
    canCfg.timeStampClkDiv = CAN_TIME_STAMP_CLK_DIV_8;   /* 时间戳时钟源分频 */
    canCfg.timeStampPos = CAN_TIME_STAMP_SOF;            /* 时间区采样位置选择 */
#endif
    canCfg.dmaRecvEn = false;                                   /* 不使用DMA */
    canCfg.memEccEn = false;                                    /* 失能ECC */
    canCfg.wakeupIntEn = false;                                 /* 失能唤醒功能 */
    canCfg.busOffRecDisable = false;                            /* 失能自动总线关闭恢复， busoff产生后， 会遵循ISO 11898-规范 */
    canCfg.interruptMask = CAN_IRQ_ALL_ENABLE_MSK;              /* 使能所有中断*/
    canCfg.bitrate = CAN_GetNormalBitrate(bitrateIndex);        /* 低速波特率配置*/
    canCfg.filterList = (can_filter_config_t *)s_canFilterList; /* 过滤器配置*/
    canCfg.callback = (can_callback_t)CAN_EventCallback;

    CAN_InitGPIO(instance);                /* CAN模块IO口初始化 */
    GPIO_LedInit();                        /* LED IO口初始化 */
    (void)CAN_DRV_Init(instance, &canCfg); /* CAN模块初始化 */

    while (1)
    {
        CAN_SendTest(instance, type); /* 发送数据测试 */
        OSIF_TimeDelay(500);
        LED3_TOGGLE; /* LED3 翻转 */
    }
}
