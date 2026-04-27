#ifndef _GPIO_H
#define _GPIO_H

/*!
 * @file gpio.h
 *
 * @brief gpio function definition.
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include <stdbool.h>
#include <stdint.h>
#include "ac7840x.h"
#include "gpio_drv.h"
#include "port_hw.h"

/* ============================================  Define  ============================================ */
// LED定义
#define LED1_PORT (PORTE)
#define LED1_GPIO (GPIOE)
#define LED1_PIN (16)

#define LED2_PORT (PORTE)
#define LED2_GPIO (GPIOE)
#define LED2_PIN (15)

#define LED3_PORT (PORTD)
#define LED3_GPIO (GPIOD)
#define LED3_PIN (1)

// ATA5296 GPIO定义
#define NRES_PORT (PORTE)
#define NRES_GPIO (GPIOE)
#define NRES_PIN (11)

#define IRQ_PORT (PORTB)
#define IRQ_GPIO (GPIOB)
#define IRQ_PIN (28)

#define IO0_PORT (PORTB)
#define IO0_GPIO (GPIOB)
#define IO0_PIN (29)

#define LFCLK_PORT (PORTB)
#define LFCLK_GPIO (GPIOB)
#define LFCLK_PIN (27)

// KEY定义
#define KEY1_PORT (PORTA)
#define KEY1_GPIO (GPIOA)
#define KEY1_PIN (9)

#define KEY2_PORT (PORTA)
#define KEY2_GPIO (GPIOA)
#define KEY2_PIN (8)

// RF 引脚定义
#define RFSHUT_PORT (PORTE)
#define RFSHUT_GPIO (GPIOE)
#define RFSHUT_PIN (0)

#define RFDATA_PORT (PORTC)
#define RFDATA_GPIO (GPIOC)
#define RFDATA_PIN (5)
// door_control
#define Lockopen_PORT (PORTD)
#define Lockopen_GPIO (GPIOD)
#define Lockopen_PIN (7)

#define Lockcolse_PORT (PORTC)
#define Lockcolse_GPIO (GPIOC)
#define Lockcolse_PIN (2)

#define Lockmos_PORT (PORTC)
#define Lockmos_GPIO (GPIOC)
#define Lockmos_PIN (3)
// LED状态定义
#define LED1_ON                                \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED1_GPIO, LED1_PIN, 0); \
  } while (0)
#define LED1_OFF                               \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED1_GPIO, LED1_PIN, 1); \
  } while (0)
#define LED1_TOGGLE                                  \
  do                                                 \
  {                                                  \
    GPIO_DRV_TogglePins(LED1_GPIO, (1 << LED1_PIN)); \
  } while (0)

#define LED2_ON                                \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED2_GPIO, LED2_PIN, 0); \
  } while (0)
#define LED2_OFF                               \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED2_GPIO, LED2_PIN, 1); \
  } while (0)
#define LED2_TOGGLE                                  \
  do                                                 \
  {                                                  \
    GPIO_DRV_TogglePins(LED2_GPIO, (1 << LED2_PIN)); \
  } while (0)

#define LED3_ON                                \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED3_GPIO, LED3_PIN, 0); \
  } while (0)
#define LED3_OFF                               \
  do                                           \
  {                                            \
    GPIO_DRV_WritePin(LED3_GPIO, LED3_PIN, 1); \
  } while (0)
#define LED3_TOGGLE                                  \
  do                                                 \
  {                                                  \
    GPIO_DRV_TogglePins(LED3_GPIO, (1 << LED3_PIN)); \
  } while (0)

#define GET_K1_STS() (GPIO_DRV_ReadPins(KEY1_GPIO) & (1 << KEY1_PIN))
#define GET_K2_STS() (GPIO_DRV_ReadPins(KEY2_GPIO) & (1 << KEY2_PIN))
#define DIR_IN (0)
#define DIR_OUT (1)
#define LOW (0)
#define HIGH (1)

  /* ===========================================  Typedef  ============================================ */

  /* ==========================================  Variables  =========================================== */

  /* ====================================  Functions declaration  ===================================== */
  extern void GPIO_LedInit(void);
  extern void GPIO_KeyInit(void);
  void GPIO_Init(void);
  uint8_t Get_KeyValue(void);
  /* ======================================  Functions define  ======================================== */

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_H */

/* =============================================  EOF  ============================================== */
