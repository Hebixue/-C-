#ifndef PKES_CORE_H_
#define PKES_CORE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化 PKES 业务状态，例如默认 LF 天线编号和指示灯节拍。 */
void PKES_Core_Init(void);

/* 将继电器控制状态转换为 0x301 Byte5 的门锁状态。 */
uint8_t PKES_Core_GetCanLockState(void);

/* 门把手触发后开始一次四天线定位窗口，region_code 为触发门把手区域。 */
void PKES_Core_StartHandleRanging(uint8_t region_code);

/* 门把手触发定位窗口结束。 */
void PKES_Core_EndHandleRanging(void);

/* 在定位窗口结束前兜底判区：缺失天线按超远距离参与融合。 */
void PKES_Core_FinalizeHandleRanging(uint32_t now_ms);

/* 处理 LF 触发事件：发送 LF 唤醒并切换下一根天线。 */
void PKES_Core_HandleLFEvent(void);

/* 周期性维护非阻塞状态：LED、继电器脉冲和待机心跳。 */
void PKES_Core_UpdateTick(void);

/* 处理一帧 5 字节 RF 数据：校验 CRC、区分主动按键帧和被动 RSSI 帧，并上报 CAN。 */
void PKES_Core_ProcessRFData(const uint8_t buf[5]);

#ifdef __cplusplus
}
#endif

#endif /* PKES_CORE_H_ */
