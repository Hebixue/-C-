#ifndef PKES_CAN_APP_H_
#define PKES_CAN_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void CAN_App_Init(void);

/* 非阻塞发送任务：主循环中应尽量频繁调用，用于处理事件帧重复发送。 */
void CAN_App_Task(void);

void CAN_App_SendSysState(uint8_t sys_state,
                          uint8_t status_code,
                          uint8_t region_code,
                          uint8_t antenna_id,
                          uint8_t lock_state,
                          uint8_t trigger_src,
                          uint8_t flags);

void CAN_App_SendKeyRssi(uint8_t key_id,
                         uint16_t rssi,
                         uint8_t rssi_ant);

void CAN_App_SendDistance(uint8_t antenna_id,
                          uint16_t dist_cm,
                          uint8_t region_code,
                          uint8_t confidence,
                          uint8_t sample_cnt);

#ifdef __cplusplus
}
#endif

#endif /* PKES_CAN_APP_H_ */
