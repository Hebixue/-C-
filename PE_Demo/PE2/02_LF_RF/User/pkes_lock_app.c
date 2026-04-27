#include "pkes_lock_app.h"

#include "gpio.h"

/* 继电器脉冲持续节拍数。当前项目的调用节拍由 main.c 决定。 */
#define LOCK_APP_PULSE_TICKS 1u

static uint8_t s_lock_pulse_ticks;
static uint8_t s_lock_state;

/* 关闭开锁继电器、闭锁继电器和 MOS 供电使能。 */
static void Lock_App_AllOff(void)
{
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 0);
}

void Lock_App_Init(void)
{
    s_lock_pulse_ticks = 0u;
    s_lock_state = PKES_LOCK_APP_IDLE;
    Lock_App_AllOff();
}

void Lock_App_UnlockPulse(void)
{
    /* 开锁动作：开锁继电器吸合，闭锁继电器断开，MOS 供电使能。 */
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 1);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 1);

    s_lock_pulse_ticks = LOCK_APP_PULSE_TICKS;
    s_lock_state = PKES_LOCK_APP_UNLOCKING;
}

void Lock_App_LockPulse(void)
{
    /* 闭锁动作：闭锁继电器吸合，开锁继电器断开，MOS 供电使能。 */
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 1);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 1);

    s_lock_pulse_ticks = LOCK_APP_PULSE_TICKS;
    s_lock_state = PKES_LOCK_APP_LOCKING;
}

void Lock_App_Stop(void)
{
    s_lock_pulse_ticks = 0u;
    s_lock_state = PKES_LOCK_APP_IDLE;
    Lock_App_AllOff();
}

void Lock_App_UpdateTick(void)
{
    if (s_lock_pulse_ticks == 0u)
    {
        return;
    }

    s_lock_pulse_ticks--;
    if (s_lock_pulse_ticks == 0u)
    {
        Lock_App_AllOff();
        s_lock_state = PKES_LOCK_APP_IDLE;
    }
}

uint8_t Lock_App_GetState(void)
{
    return s_lock_state;
}
