#ifndef PKES_LOCK_APP_H_
#define PKES_LOCK_APP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PKES_LOCK_APP_IDLE = 0u,
    PKES_LOCK_APP_UNLOCKING = 1u,
    PKES_LOCK_APP_LOCKING = 2u
} pkes_lock_app_state_t;

/* 初始化继电器控制状态，并关闭开锁、闭锁和 MOS 使能输出。 */
void Lock_App_Init(void);

/* 触发一次开锁脉冲：Lockopen=1，Lockcolse=0，Lockmos=1。 */
void Lock_App_UnlockPulse(void);

/* 触发一次闭锁脉冲：Lockopen=0，Lockcolse=1，Lockmos=1。 */
void Lock_App_LockPulse(void);

/* 立即停止继电器输出，三路控制脚全部拉低。 */
void Lock_App_Stop(void);

/* 定时节拍更新函数；主循环应在固定节拍中调用，用于自动撤销继电器脉冲。 */
void Lock_App_UpdateTick(void);

/* 读取当前继电器控制状态，返回 pkes_lock_app_state_t。 */
uint8_t Lock_App_GetState(void);

#ifdef __cplusplus
}
#endif

#endif /* PKES_LOCK_APP_H_ */
