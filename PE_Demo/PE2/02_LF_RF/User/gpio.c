/* ===========================================  Includes  =========================================== */
#include "gpio.h"
#include "spi_master_drv.h"
#include "spi.h"
/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/*!
 * @brief GPIO��ʼ��
 *
 * @param[in] none
 *
 * @return none
 */
void GPIO_Inits(void)
{
    // ATA5293
    //  NRES
    GPIO_DRV_SetMuxModeSel(NRES_PORT, NRES_PIN, PORT_MUX_AS_GPIO);        /*NRES���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(NRES_GPIO, NRES_PIN, GPIO_OUTPUT_DIRECTION); /*NRES����GPIOΪ���*/
    GPIO_DRV_WritePin(NRES_GPIO, NRES_PIN, 0);
    // IQR
    GPIO_DRV_SetMuxModeSel(IRQ_PORT, IRQ_PIN, PORT_MUX_AS_GPIO);             /*IQR*/
    GPIO_DRV_SetPinDirection(IRQ_GPIO, IRQ_PIN, GPIO_INPUT_DIRECTION);       /*IRQ����GPIOΪ����*/
    GPIO_DRV_SetPullSel(IRQ_PORT, IRQ_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*IRQ内部下拉*/
    // IO0
    GPIO_DRV_SetMuxModeSel(IO0_PORT, IO0_PIN, PORT_MUX_AS_GPIO);             /*IO0*/
    GPIO_DRV_SetPinDirection(IO0_GPIO, IO0_PIN, GPIO_INPUT_DIRECTION);       /*IO0����GPIOΪ����*/
    GPIO_DRV_SetPullSel(IO0_PORT, IO0_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*IO0�ڲ�����*/
    // LFCLK
    GPIO_DRV_SetMuxModeSel(LFCLK_PORT, LFCLK_PIN, PORT_MUX_AS_GPIO);             /*LFCLK*/
    GPIO_DRV_SetPinDirection(LFCLK_GPIO, LFCLK_PIN, GPIO_INPUT_DIRECTION);       /*LFCLK����GPIOΪ����*/
    GPIO_DRV_SetPullSel(LFCLK_PORT, LFCLK_PIN, PORT_INTERNAL_PULL_DOWN_ENABLED); /*LFCLK�ڲ�����*/
    // CMT2220LS
    // RFSHUT
    GPIO_DRV_SetMuxModeSel(RFSHUT_PORT, RFSHUT_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(RFSHUT_GPIO, RFSHUT_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT����GPIOΪ���*/
    GPIO_DRV_WritePin(RFSHUT_GPIO, RFSHUT_PIN, 0);                            /*1���ضϣ�0������*/
    // RFDATA
    GPIO_DRV_SetMuxModeSel(RFDATA_PORT, RFDATA_PIN, PORT_MUX_AS_GPIO);            /*RFDATA���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(RFDATA_GPIO, RFDATA_PIN, GPIO_INPUT_DIRECTION);      /*RFDATA����GPIOΪ����*/
    GPIO_DRV_SetPullSel(RFDATA_PORT, RFDATA_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*RFDATA����*/
    // door_control
    // Lockopen
    GPIO_DRV_SetMuxModeSel(Lockopen_PORT, Lockopen_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(Lockopen_GPIO, Lockopen_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT����GPIOΪ���*/
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    // Lockcolse
    GPIO_DRV_SetMuxModeSel(Lockcolse_PORT, Lockcolse_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(Lockcolse_GPIO, Lockcolse_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT����GPIOΪ���*/
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    // lockmos
    GPIO_DRV_SetMuxModeSel(Lockmos_PORT, Lockmos_PIN, PORT_MUX_AS_GPIO);        /*RFSHUT���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(Lockmos_GPIO, Lockmos_PIN, GPIO_OUTPUT_DIRECTION); /*RFSHUT����GPIOΪ���*/
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 0);
}

/*!
 * @brief  ��ʼ��LED����
 *
 * @param none
 * @return none
 */
void GPIO_LedInit(void)
{
    /*��ʼ�����Ź��ܣ��в��������ϵ�Ĭ��Ϊ��GPIO������ѡ���书��ΪGPIO������ΪGPIOʹ��*/
    GPIO_DRV_SetMuxModeSel(LED1_PORT, LED1_PIN, PORT_MUX_AS_GPIO); /*���ܸ���ѡ��*/
    GPIO_DRV_SetMuxModeSel(LED2_PORT, LED2_PIN, PORT_MUX_AS_GPIO);
    GPIO_DRV_SetMuxModeSel(LED3_PORT, LED3_PIN, PORT_MUX_AS_GPIO);

    GPIO_DRV_SetPinDirection(LED1_GPIO, LED1_PIN, GPIO_OUTPUT_DIRECTION); /*����GPIOΪ���*/
    GPIO_DRV_SetPinDirection(LED2_GPIO, LED2_PIN, GPIO_OUTPUT_DIRECTION);
    GPIO_DRV_SetPinDirection(LED3_GPIO, LED3_PIN, GPIO_OUTPUT_DIRECTION);

    /*�ϵ�Ĭ��LED3��*/
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
}

/*!
 * @brief  ��ʼ����������
 *
 * @param none
 * @return none
 */
void GPIO_KeyInit(void)
{
    /*��ʼ�����Ź��ܣ��в��������ϵ�Ĭ��Ϊ��GPIO������ѡ���书��ΪGPIO������ΪGPIOʹ��*/
    // KEY1
    GPIO_DRV_SetMuxModeSel(KEY1_PORT, KEY1_PIN, PORT_MUX_AS_GPIO);            /*���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(KEY1_GPIO, KEY1_PIN, GPIO_INPUT_DIRECTION);      /*���ð�������Ϊ����*/
    GPIO_DRV_SetPullSel(KEY1_PORT, KEY1_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*ʹ�ܰ�����������Ϊ����*/

    // KEY2
    GPIO_DRV_SetMuxModeSel(KEY2_PORT, KEY2_PIN, PORT_MUX_AS_GPIO);            /*���ܸ���ѡ��*/
    GPIO_DRV_SetPinDirection(KEY2_GPIO, KEY2_PIN, GPIO_INPUT_DIRECTION);      /*���ð�������Ϊ����*/
    GPIO_DRV_SetPullSel(KEY2_PORT, KEY2_PIN, PORT_INTERNAL_PULL_NOT_ENABLED); /*ʹ�ܰ�����������Ϊ����*/
}
///*!
// * @brief  ��������״̬���
// *
// * @param none
// * @return none
// */
// uint8_t Get_KeyValue(void)
//{
//    uint8_t keyvalue = 0;
//    if (GET_K1_STS() == 0) // �����Ϊ�͵�ƽ���������£�
//    {
//        OSIF_TimeDelay(10);   // ����
//        if (GET_K1_STS() == 0) // �ٴ�ȷ��
//        {
//            while (GET_K1_STS() == 0)
//                ;         // �ȴ������ͷ�
//            keyvalue = 1; // ��ǰ�������
//        }
//    }
//    return keyvalue;
//}
/*!
 * @brief GPIO����
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

