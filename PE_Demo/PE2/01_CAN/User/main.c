#include "ac7840x.h"
#include "system_ac7840x.h"
#include "debugout_ac7840x.h"
#include "CAN.h"
#include "string.h"
#include "ckgen_hw.h"
#include "clock_config.h"
#include "spi.h"
#include "gpio.h"
#include "can.h"
#include "LF.h"
#include "RF.h"
#include "fortissimo.h"
#include "timer.h"
/*!
 *例程说明：
 *例程CAN 模块基本操作
 *定时发送CAN报文
 */
static uint8_t rf_data[3];
/*!
 * @brief 时钟配置函数.
 *
 * @param[in] none
 *
 * @return none
 */
void SystemClock_Config(void)
{
    CKGEN_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CKGEN_DRV_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_AGREEMENT);
}
/*!
 * @brief main
 *
 * @param[in]  none
 *
 * @return 0: success, other: error value
 */
int main(void)
{

    /*时钟配置*/
    SystemClock_Config();
    InitDebug();
		GPIO_Init();
		CAN_Init(0);
		rf_data[0]=0x11;
		rf_data[1]=0x22;
		rf_data[2]=0x33;
    /* CAN Test*/
	while (1)
    {
        CAN_SendData(0, 0x100, rf_data, 3);                              /* 发送数据测试 */
        OSIF_TimeDelay(500);
        LED3_TOGGLE;                                               /* LED3 翻转 */
    }
    


}
