
/* ===========================================  Includes  =========================================== */
#include "clock_config.h"

/* *************************************************************************
 * Configuration structure for peripheral clock configuration 0
 * ************************************************************************* */
/*! @brief peripheral clock configuration 0 */
peripheral_clock_config_t peripheralClockConfig0[NUM_OF_PERIPHERAL_CLOCKS_0] = {
    {
        .clockName = I2C0_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = TIMER_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = SPI0_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = SPI1_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = SPI2_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = ADC0_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = ADC1_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = CAN0_CLK,
        .clkSrc = CORE_CLK,
        .divider = DIVIDE_BY_TWO,
    },
    {
        .clockName = CAN1_CLK,
        .clkSrc = CORE_CLK,
        .divider = DIVIDE_BY_TWO,
    },
    {
        .clockName = CAN2_CLK,
        .clkSrc = CORE_CLK,
        .divider = DIVIDE_BY_TWO,
    },
    {
        .clockName = CAN3_CLK,
        .clkSrc = CORE_CLK,
        .divider = DIVIDE_BY_TWO,
    },
    {
        .clockName = PCT_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = EIO_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = UART0_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = UART1_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = UART2_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = UART3_CLK,
        .clkSrc = SPLLDIV2_CLK,
    },
    {
        .clockName = PWM0_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
    {
        .clockName = PWM1_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
    {
        .clockName = PWM2_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
    {
        .clockName = PWM3_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
    {
        .clockName = PWM4_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
    {
        .clockName = PWM5_CLK,
        .clkSrc = HSIDIV1_CLK,
    },
};

/* *************************************************************************
 * Configuration structure for Clock Configuration 0
 * ************************************************************************* */
/*! @brief User Configuration structure clockMan1_InitConfig0 */
clock_manager_user_config_t clockMan1_InitConfig0 = {
    .ckgenConfig =
        {
            .hsiConfig =
                {
                    .initialize = true,       /*!< Initialize */
                    .enableInLowPower = true, /*!< Enable in VLPS mode */
                    /* HSIDIV */
                    .div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< HSIDIV1    */
                    .div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< HSIDIV2    */
                },
            .vhsiConfig =
                {
                    .initialize = true, /*!< Initialize */
                    /* VHSIDIV */
                    .div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< VHSIDIV1   */
                    .div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< VHSIDIV2   */
                },
            .rtcConfig =
                {
                    .initialize = true, /*!< Initialize */
                    .rtcClkInFreq = 0U, /*!< RTC_CLKIN  */
                },
            .hseConfig =
                {
                    .initialize = true, /*!< Initialize */
                    .freq = 8000000U,   /*!< Frequency  */
                    /* HSEREF */
                    .extRef = CKGEN_HSE_REF_OSC, /*!< HSEREF     */
                    /* HSEDIV */
                    .div1 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< HSEDIV1    */
                    .div2 = CKGEN_ASYNC_CLOCK_DIV_BY_1, /*!< HSEDIV2    */
                },
            .spllConfig =
                {
                    .initialize = true, /*!< Initialize */
                    /* SPLLCFG */
                    .prediv = (uint8_t)CKGEN_SPLL_CLOCK_PREDIV_BY_1, /*!< PREDIV     */
                    .fbkdiv = 120U,                                  /*!< FBKDIV     */
                    .posdiv = 4U,                                    /*!< POSDIV     */
                    .src = (uint8_t)CKGEN_SPLL_REF_HSE,              /*!< SOURCE     */
                    /* SPLLDIV */
                    .div1 = CKGEN_ASYNC_CLOCK_DIV_BY_2, /*!< SPLLDIV1   */
                    .div2 = CKGEN_ASYNC_CLOCK_DIV_BY_2, /*!< SPLLDIV2   */
                },
            .clockModeConfig =
                {
                    .initialize = true, /*!< Initialize */
                    .rccrConfig =       /*!< RCCR - Run Clock Control Register  */
                    {
                        .src = CKGEN_SYSTEM_CLOCK_SRC_SPLL,     /*!< SCS        */
                        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1, /*!< DIVCORE    */
                        .divBus = CKGEN_SYSTEM_CLOCK_DIV_BY_2,  /*!< DIVBUS     */
                    },
                    .vccrConfig = /*!< VCCR - VLPR Clock Control Register */
                    {
                        .src = CKGEN_SYSTEM_CLOCK_SRC_HSI,      /*!< SCS        */
                        .divCore = CKGEN_SYSTEM_CLOCK_DIV_BY_1, /*!< DIVCORE    */
                        .divBus = CKGEN_SYSTEM_CLOCK_DIV_BY_2,  /*!< DIVBUS     */
                    },
                },
        },
    .pccConfig =
        {
            .peripheralClocks = peripheralClockConfig0, /*!< Peripheral clock control configurations  */
            .count = NUM_OF_PERIPHERAL_CLOCKS_0,        /*!< Number of the peripheral clock control configurations  */
        },
    .simConfig =
        {
            .clockOutConfig = /*!< Clock Out Configuration.           */
            {
                .initialize = true, /*!< Initialize */
                .enable = false,
                .source = CKGEN_CLKOUT_SEL_HSE_CLK, /*!< CLKOUTSEL  */
                .divider = CKGEN_CLKOUT_DIV_BY_2,   /*!< CLKOUTDIV  */
            },
            .lpClockConfig = /*!< Low Power Clock Configuration.     */
            {
                .initialize = true,                   /*!< Initialize */
                .sourceLsiClk = LSI_CLK_SEL_128K,     /*!< LSICLKSEL  */
                .sourceRtcClk = RTC_CLK_SEL_VHSIDIV1, /*!< RTCCLKSEL  */
            },
            .tclkConfig = /*!< Tclk Clock Configuration.          */
            {
                .initialize = true,  /*!< Initialize */
                .tclkFreq[0U] = 0UL, /*!< TCLK0      */
                .tclkFreq[1U] = 0UL, /*!< TCLK1      */
                .tclkFreq[2U] = 0UL, /*!< TCLK2      */
            },
            .traceClockConfig = /*!< Debug Trace Clock Configuration.   */
            {
                .initialize = true, /*!< Initialize */
                .divider = 0U,      /*!< TRACEDIV   */
            },
        },
};

/*! @brief Array of pointers to User configuration structures */
clock_manager_user_config_t const *g_clockManConfigsArr[] = {
    &clockMan1_InitConfig0};
/*! @brief Array of pointers to User defined Callbacks configuration structures */
clock_manager_callback_user_config_t *g_clockManCallbacksArr[] = {(void *)0};

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */

/* =============================================  EOF  ============================================== */
