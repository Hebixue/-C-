/* ===========================================  Includes  =========================================== */
#include "gpio.h"
#include "spi_master_drv.h"
#include "spi.h"
/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/*!
 * @brief GPIO初始化
 *
 * @param[in] none
 *
 * @return none
 */
void GPIO_Inits(void)
{
    // ATA5296
    //  NRES
    GPIO_DRV_SetMuxModeSel(NRES_PORT, NRES_PIN, PORT_MUX_AS_GPIO);        /*NRES功能复用选择*/
    GPIO_DRV_SetPinDirection(NRES_GPIO, NRES_PIN, GPIO_OUTPUT_DIRECTION); /*NRES设置GPIO为输出*/
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 0);
    // IQR
    GPIO_DRV_SetMuxModeSel(IRQ_PORT, IRQ_PIN, PORT_MUX_AS_GPIO);             /*IQR*/
    GPIO_DRV_SetPinDirection(IRQ_GPIO, IRQ_PIN, GPIO_INPUT_DIRECTION);       /*IRQ设置GPIO为输入*/
    GPIO_DRV_SetPullSel(IRQ_PORT, IRQ_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*IRQ内部下拉*/
    // IO0
    GPIO_DRV_SetMuxModeSel(IO0_PORT, IO0_PIN, PORT_MUX_AS_GPIO);             /*IO0*/
    GPIO_DRV_SetPinDirection(IO0_GPIO, IO0_PIN, GPIO_INPUT_DIRECTION);       /*IO0设置GPIO为输入*/
    GPIO_DRV_SetPullSel(IO0_PORT, IO0_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*IO0内部下拉*/
    // LFCLK
    GPIO_DRV_SetMuxModeSel(LFCLK_PORT, LFCLK_PIN, PORT_MUX_AS_GPIO);             /*LFCLK*/
    GPIO_DRV_SetPinDirection(LFCLK_GPIO, LFCLK_PIN, GPIO_INPUT_DIRECTION);       /*LFCLK设置GPIO为输入*/
    GPIO_DRV_SetPullSel(LFCLK_PORT, LFCLK_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*LFCLK内部下拉*/
    // CMT2220LS
    // RFSHUT
    GPIO_DRV_SetMuxModeSel(RFSHUT_PORT, RFSHUT_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT功能复用选择*/
    GPIO_DRV_SetPinDirection(RFSHUT_GPIO, RFSHUT_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT设置GPIO为输出*/
    GPIO_DRV_WritePin(RFSHUT_GPIO, RFSHUT_PIN, 0);                            /*1：关断；0：工作*/
    // RFDATA
    GPIO_DRV_SetMuxModeSel(RFDATA_PORT, RFDATA_PIN, PORT_MUX_AS_GPIO);            /*RFDATA功能复用选择*/
    GPIO_DRV_SetPinDirection(RFDATA_GPIO, RFDATA_PIN, GPIO_INPUT_DIRECTION);      /*RFDATA设置GPIO为输入*/
    GPIO_DRV_SetPullSel(RFDATA_PORT, RFDATA_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*RFDATA悬空*/
    // door_control
    // Lockopen
    GPIO_DRV_SetMuxModeSel(Lockopen_PORT, Lockopen_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT功能复用选择*/
    GPIO_DRV_SetPinDirection(Lockopen_GPIO, Lockopen_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT设置GPIO为输出*/
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    // Lockcolse
    GPIO_DRV_SetMuxModeSel(Lockcolse_PORT, Lockcolse_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT功能复用选择*/
    GPIO_DRV_SetPinDirection(Lockcolse_GPIO, Lockcolse_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT设置GPIO为输出*/
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    // lockmos
    GPIO_DRV_SetMuxModeSel(Lockmos_PORT, Lockmos_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT功能复用选择*/
    GPIO_DRV_SetPinDirection(Lockmos_GPIO, Lockmos_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT设置GPIO为输出*/
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 0);
}

/*!
 * @brief  初始化LED引脚
 *
 * @param none
 * @return none
 */
void GPIO_LedInit(void)
{
    /*初始化引脚功能，有部分引脚上电默认为非GPIO，必须选择其功能为GPIO才能作为GPIO使用*/
    GPIO_DRV_SetMuxModeSel(LED1_PORT, LED1_PIN, PORT_MUX_AS_GPIO); /*功能复用选择*/
    GPIO_DRV_SetMuxModeSel(LED2_PORT, LED2_PIN, PORT_MUX_AS_GPIO);
    GPIO_DRV_SetMuxModeSel(LED3_PORT, LED3_PIN, PORT_MUX_AS_GPIO);

    GPIO_DRV_SetPinDirection(LED1_GPIO, LED1_PIN, GPIO_OUTPUT_DIRECTION); /*设置GPIO为输出*/
    GPIO_DRV_SetPinDirection(LED2_GPIO, LED2_PIN, GPIO_OUTPUT_DIRECTION);
    GPIO_DRV_SetPinDirection(LED3_GPIO, LED3_PIN, GPIO_OUTPUT_DIRECTION);

    /*上电默认LED3打开*/
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
}

/*!
 * @brief  初始化按键引脚
 *
 * @param none
 * @return none
 */
void GPIO_KeyInit(void)
{
    /*初始化引脚功能，有部分引脚上电默认为非GPIO，必须选择其功能为GPIO才能作为GPIO使用*/
    // KEY1
    GPIO_DRV_SetMuxModeSel(KEY1_PORT, KEY1_PIN, PORT_MUX_AS_GPIO);            /*功能复用选择*/
    GPIO_DRV_SetPinDirection(KEY1_GPIO, KEY1_PIN, GPIO_INPUT_DIRECTION);      /*设置按键引脚为输入*/
    GPIO_DRV_SetPullSel(KEY1_PORT, KEY1_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*使能按键引脚设置为悬空*/

    // KEY2
    GPIO_DRV_SetMuxModeSel(KEY2_PORT, KEY2_PIN, PORT_MUX_AS_GPIO);            /*功能复用选择*/
    GPIO_DRV_SetPinDirection(KEY2_GPIO, KEY2_PIN, GPIO_INPUT_DIRECTION);      /*设置按键引脚为输入*/
    GPIO_DRV_SetPullSel(KEY2_PORT, KEY2_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*使能按键引脚设置为悬空*/
}
///*!
// * @brief  按键引脚状态检测
// *
// * @param none
// * @return none
// */
// uint8_t Get_KeyValue(void)
//{
//    uint8_t keyvalue = 0;
//    if (GET_K1_STS() == 0) // 若按键为低电平（按键按下）
//    {
//        OSIF_TimeDelay(10);   // 消抖
//        if (GET_K1_STS() == 0) // 再次确认
//        {
//            while (GET_K1_STS() == 0)
//                ;         // 等待按键释放
//            keyvalue = 1; // 标记按键按下
//        }
//    }
//    return keyvalue;
//}
/*!
 * @brief GPIO配置
 *
 * @param none
 *
 * @return none
 */
void GPIO_Init(void)
{
    GPIO_LedInit();
    GPIO_KeyInit();
    GPIO_Inits();
}

/* ======================================  Functions define  ======================================== */

/* =============================================  EOF  ============================================== */
