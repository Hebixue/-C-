/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of AutoChips Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2024. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */
/******************************************************************************
 * @version: V3.1.0  AC7840x  Demo
 ******************************************************************************/

/*!
* @file clock_config.c
*
* @brief This file provides clock config integration functions.
*
*/

/* ===========================================  Includes  =========================================== */
#include "clock_config.h"

/* ============================================  Define  ============================================ */


/* ===========================================  Typedef  ============================================ */


/* ==========================================  Variables  =========================================== */

/*! @brief peripheral clock configuration 0 */
peripheral_clock_config_t peripheralClockConfig0[NUM_OF_PERIPHERAL_CLOCKS_0] = {
    {
        .clockName        = I2C0_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = TIMER_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = SPI0_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = SPI1_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = SPI2_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = ADC0_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = ADC1_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = CAN0_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = CAN1_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = CAN2_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = CAN3_CLK,
        .clkSrc           = CORE_CLK,
        .divider          = DIVIDE_BY_TWO,
    },
    {
        .clockName        = PCT_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = EIO_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = UART0_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = UART1_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = UART2_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = UART3_CLK,
        .clkSrc           = SPLLDIV2_CLK,
    },
    {
        .clockName        = PWM0_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
    {
        .clockName        = PWM1_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
    {
        .clockName        = PWM2_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
    {
        .clockName        = PWM3_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
    {
        .clockName        = PWM4_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
    {
        .clockName        = PWM5_CLK,
        .clkSrc           = SPLLDIV1_CLK,
    },
};

/*! @brief User Configuration structure clockMan1_InitConfig0 */
clock_manager_user_config_t clockMan1_InitConfig0 = {
    /* CKGEN Clock Configuration. */
    .ckgenConfig =
    {
        /* Slow Internal Reference Clock Configuration. */
        .hsiConfig =
        {
            /* HSIDIV */
            .div1               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< HSIDIV1    */
            .div2               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< HSIDIV2    */

            .initialize         = true,                                         /*!< Initialize */
            .enableInLowPower   = true,                                         /*!< Enable in VLPS mode */
        },
        /* Fast Internal Reference Clock Configuration. */
        .vhsiConfig =
        {
            /* VHSIDIV */
            .div1               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< VHSIDIV1   */
            .div2               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< VHSIDIV2   */

            .initialize         = true,                                         /*!< Initialize */
        },
        /* System Oscillator Configuration. */
        .hseConfig =
        {
            .freq               = 8000000U,                                     /*!< Frequency  */
            .monitor            = CKGEN_XOSC_MONITOR_RESET,
            /* HSEREF */
            .extRef             = CKGEN_HSE_REF_OSC,                            /*!< HSEREF     */
            /* HSEDIV */
            .div1               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< HSEDIV1    */
            .div2               = CKGEN_ASYNC_CLOCK_DIV_BY_1,                   /*!< HSEDIV2    */
            .initialize         = true,                                         /*!< Initialize */
        },
        /* System Phase Locked Loop Configuration. */
        .spllConfig =
        {
            /* SPLLCFG */
            .prediv             = (uint8_t)CKGEN_SPLL_CLOCK_PREDIV_BY_1,        /*!< PREDIV     */
            .fbkdiv             = 120U,                                         /*!< FBKDIV     */
            .posdiv             = 4U,                                           /*!< POSDIV     */
            .src                = (uint8_t)CKGEN_SPLL_REF_HSE,                  /*!< SOURCE     */
            .lockDetect         = CKGEN_SPLL_LOCK_DETECT_RESET,
            /* SPLLDIV */
            .div1               = CKGEN_ASYNC_CLOCK_DIV_BY_2,                   /*!< SPLLDIV1   */
            .div2               = CKGEN_ASYNC_CLOCK_DIV_BY_2,                   /*!< SPLLDIV2   */
            .initialize         = true,                                         /*!< Initialize */
        },
        /* RTC Configuration.*/
        .rtcConfig =
        {
            .rtcClkInFreq       = 0U,                                           /*!< RTC_CLKIN  */
            .initialize         = true,                                         /*!< Initialize */
        },
        /* Clock Mode Configuration.*/
        .clockModeConfig =
        {
            /* RCCR - Run Clock Control Register. */
            .rccrConfig =
            {
                .src            = CKGEN_SYSTEM_CLOCK_SRC_SPLL,                  /*!< SCS        */
                .divCore        = CKGEN_SYSTEM_CLOCK_DIV_BY_1,                  /*!< DIVCORE    */
                .divBus         = CKGEN_SYSTEM_CLOCK_DIV_BY_2,                  /*!< DIVBUS     */
            },
            /* VCCR - VLPR Clock Control Register */
            .vccrConfig =
            {
                .src            = CKGEN_SYSTEM_CLOCK_SRC_HSI,                   /*!< SCS        */
                .divCore        = CKGEN_SYSTEM_CLOCK_DIV_BY_1,                  /*!< DIVCORE    */
                .divBus         = CKGEN_SYSTEM_CLOCK_DIV_BY_2,                  /*!< DIVBUS     */
            },
            .initialize         = true,                                         /*!< Initialize */
        },
    },

    .simConfig =
    {
        /* Clock Out Configuration. */
        .clockOutConfig =
        {
            .initialize         = true,                                         /*!< Initialize */
            .enable             = false,
            .source             = CKGEN_CLKOUT_SEL_HSI_CLK,                     /*!< CLKOUTSEL  */
            .divider            = CKGEN_CLKOUT_DIV_BY_2,                        /*!< CLKOUTDIV  */
        },
        /* Low Power Clock Configuration. */
        .lpClockConfig =
        {
            .initialize         = true,                                         /*!< Initialize */
            .sourceLsiClk       = LSI_CLK_SEL_128K,                             /*!< LSICLKSEL  */
            .sourceRtcClk       = RTC_CLK_SEL_VHSIDIV1,                         /*!< RTCCLKSEL  */
        },
        /* Tclk Clock Configuration. */
        .tclkConfig =
        {
            .initialize         = true,                                         /*!< Initialize */
            .tclkFreq[0U]       = 0UL,                                          /*!< TCLK0      */
            .tclkFreq[1U]       = 0UL,                                          /*!< TCLK1      */
            .tclkFreq[2U]       = 0UL,                                          /*!< TCLK2      */
        },
        /* Debug Trace Clock Configuration. */
        .traceClockConfig =
        {
            .initialize       = true,                                           /*!< Initialize */
            .divider          = 0U,                                             /*!< TRACEDIV   */
        },
    },

    /* PCC - Peripheral Clock Configuration. */
    .pccConfig =
    {
        .peripheralClocks = peripheralClockConfig0,                             /*!< Peripheral clock control configurations               */
        .count = NUM_OF_PERIPHERAL_CLOCKS_0,                                    /*!< Number of the peripheral clock control configurations */
    },
};

/*! @brief Array of pointers to User configuration structures */
clock_manager_user_config_t const * g_clockManConfigsArr[] = {
    &clockMan1_InitConfig0
};

/*! @brief Array of pointers to User defined Callbacks configuration structures */
clock_manager_callback_user_config_t * g_clockManCallbacksArr[] = {(void*)0};

/* ====================================  Functions declaration  ===================================== */


/* ======================================  Functions define  ======================================== */


/* =============================================  EOF  ============================================== */
