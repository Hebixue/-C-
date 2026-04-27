
/*!
 * @file timer_timeout.h
 *
 * @brief timer_timeout头文件
 */

#ifndef __TIMER_SAMPLE_H__
#define __TIMER_SAMPLE_H__
#ifdef __cplusplus
extern "C"
{
#endif

/* ===========================================  Includes  =========================================== */
#include "stdint.h"

    /* ============================================  Define  ============================================ */

    /* ===========================================  Typedef  ============================================ */

    /* ==========================================  Variables  =========================================== */
    extern uint32_t LF_timerOutFlag; /*定时标志位声明*/
    extern uint32_t RF_timerOutFlag;
    /* ====================================  Functions declaration  ===================================== */
    /*!
     * @brief  TIMER初始化函数
     *
     * @param[in] none
     * @return none
     */
    extern void RF_TIMER_Init(void);
    extern void LF_TIMER_Init(void);

    /* ======================================  Functions define  ======================================== */

#ifdef __cplusplus
}
#endif

#endif /*__TIMER_SAMPLE_H__*/
