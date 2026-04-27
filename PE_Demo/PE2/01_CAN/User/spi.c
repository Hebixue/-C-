
/*!
 * @file spi.c
 *
 * @brief This file contains the user spi function.
 *
 */

/* ===========================================  Includes  =========================================== */
#include "spi.h"
#include "spi_hw.h"
#include "spi_master_drv.h"
#include "spi_slave_drv.h"
#include "port_hw.h"
#include "gpio_hw.h"
#include "gpio.h"

/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/*! @brief SPI master configure parameter */
spi_master_config_t spiMasteConfig =
    {
        .bitsPerSec = 1000000UL,              /*! 设置SPI波特率*/
        .whichPcs = SPI_PCS1,                 /*! 设置SPI片选引脚*/
        .pcsPolarity = SPI_ACTIVE_LOW,        /*! 设置SPI片选低有效*/
        .isPcsContinuous = true,              /*! 设置片选连续模式*/
        .bitcount = 8UL,                      /*! SPI 帧长度,支持 4~32 bits*/
        .spiSrcClk = 30000000UL,              /*! SPI时钟源频率*/
        .clkPhase = SPI_CLOCK_PHASE_1ST_EDGE, /*! 设置数据采样相位,第1个边沿采样数据*/
        .clkPolarity = SPI_SCK_ACTIVE_HIGH,   /*! 设置SCK工作时极性,工作时SCK为低*/
        .msbFirst = true,                     /*! 选择从最高位开始收发*/
        .transferType = SPI_USING_POLL,       /*! 选择通信类型为轮询方式*/
        .rxDMAChannel = 1UL,                  /*! 设置SPI DMA接收通道，若未使用DMA方式传输，则该配置可忽略*/
        .txDMAChannel = 0UL,                  /*! 设置SPI DMA发送通道，若未使用DMA方式传输，则该配置可忽略*/
        .callback = NULL,                     /*! 注册回调函数 */
        .callbackParam = NULL,                /*! 回调函数参数 */
};

static spi_state_t s_spiMasterState;
// SPI_BuffersTypeDef SPI_BUFFERS;

/* ====================================  Functions declaration  ===================================== */

/*!
 * @brief SPI模块引脚初始化配置，主机模式
 *
 * @param none
 *
 * @return none
 */
static void SPI_Init_Gpio(void)
{
    GPIO_DRV_SetMuxModeSel(PORT_SPI_SCK, PIN_SPI_SCK, PORT_MUX_ALT3);   /*! SCK */
    GPIO_DRV_SetMuxModeSel(PORT_SPI_SOUT, PIN_SPI_SOUT, PORT_MUX_ALT3); /*! SOUT */
    GPIO_DRV_SetMuxModeSel(PORT_SPI_SIN, PIN_SPI_SIN, PORT_MUX_ALT3);   /*! SIN */
    GPIO_DRV_SetMuxModeSel(PORT_SPI_NCS, PIN_SPI_NCS, PORT_MUX_ALT3);   /*! NCS */
}

/*!
 * @brief SPI模块初始化配置，主机模式
 *
 * @param none
 *
 * @return none
 */
void SPI_Master_Init(void)
{
    SPI_Init_Gpio(); /*! 初始化对应引脚为SPI功能 */

    SPI_DRV_MasterInit(SPI_INSTANCE, &s_spiMasterState, &spiMasteConfig); /*! SPI主机模式初始化 */
    NVIC_SetPriority(SPI_IRQ, 2);                                         /*! 设置SPI中断优先级 */
}

/* ======================================  Functions define  ======================================== */

/* =============================================  EOF  ============================================== */
