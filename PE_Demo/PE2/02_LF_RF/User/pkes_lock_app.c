#include "pkes_lock_app.h"

#include "gpio.h"

#define LOCK_APP_LED_NONE 0u
#define LOCK_APP_LED_UNLOCKED 1u
#define LOCK_APP_LED_LOCKED 2u

static uint8_t s_lock_state;
static uint8_t s_lock_led_state;

/* 关闭开锁继电器、闭锁继电器和 MOS 供电使能。 */
static void Lock_App_AllOff(void)
{
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 0);
}

static void Lock_App_ApplyLedState(void)
{
    if (s_lock_led_state == LOCK_APP_LED_UNLOCKED)
    {
        LED3_OFF;
    }
    else
    {
        LED3_ON;
    }
}

void Lock_App_Init(void)
{
    s_lock_state = PKES_LOCK_APP_UNLOCKING;
    s_lock_led_state = LOCK_APP_LED_UNLOCKED;
    Lock_App_AllOff();
    Lock_App_ApplyLedState();
}

void Lock_App_UnlockPulse(void)
{
    /* 开锁保持：开锁继电器吸合，闭锁继电器断开，MOS 持续供电。 */
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 1);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 0);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 1);

    s_lock_state = PKES_LOCK_APP_UNLOCKING;
    s_lock_led_state = LOCK_APP_LED_UNLOCKED;
    Lock_App_ApplyLedState();
}

void Lock_App_LockPulse(void)
{
    /* 闭锁保持：闭锁继电器吸合，开锁继电器断开，MOS 持续供电。 */
    GPIO_DRV_WritePin(Lockopen_GPIO, Lockopen_PIN, 0);
    GPIO_DRV_WritePin(Lockcolse_GPIO, Lockcolse_PIN, 1);
    GPIO_DRV_WritePin(Lockmos_GPIO, Lockmos_PIN, 1);

    s_lock_state = PKES_LOCK_APP_LOCKING;
    s_lock_led_state = LOCK_APP_LED_LOCKED;
    Lock_App_ApplyLedState();
}

void Lock_App_Stop(void)
{
    s_lock_state = PKES_LOCK_APP_IDLE;
    Lock_App_AllOff();
}

void Lock_App_UpdateTick(void)
{
    /* 继电器改为保持式输出，tick 不再自动断开 MOS 或继电器。 */
}

void Lock_App_RefreshLedState(void)
{
    Lock_App_ApplyLedState();
}

uint8_t Lock_App_GetState(void)
{
    return s_lock_state;
}
