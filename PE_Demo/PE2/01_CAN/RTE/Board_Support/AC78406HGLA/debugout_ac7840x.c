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
 * AutoChips Inc. (C) 2021. All rights reserved.
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

/*!
 * @file debugout_ac7840x.c
 *
 * @brief This file provides debug information output integration functions.
 *
 */

/* PRQA S ALL EOF */
/* ===========================================  Includes  =========================================== */
#include "device_register.h"
#include "gpio_drv.h"
#include "ckgen_drv.h"
#include "debugout_ac7840x.h"
#include "../src/uart/uart_hw.h"

/* ============================================  Define  ============================================ */
#ifndef ATC_DEBUG_OUT_INFO
#define ATC_DEBUG_OUT_INFO  1
#endif

#if ATC_DEBUG_OUT_INFO
#include <stdio.h>
#include <stdarg.h>
/* define uart max receive buffer size */
#define MAX_DEBUG_BUFF_SIZE  100U
#endif

#if defined ( __CC_ARM )
    /**
    * @brief Symbol required to be defined when ARM compiler 4/5, armcc is used.
    */
    #define COMPILER_ARM_CC
#elif defined (__ARMCC_VERSION)  && (__ARMCC_VERSION >= 6010050)
    /**
    * @brief Symbol required to be defined when ARM compiler 6, armclang is used.
    */
    #define COMPILER_ARM_CLANG
#elif defined ( __GNUC__ )
    /**
    * @brief Symbol required to be defined when GNUC compiler is used.
    */
    #define COMPILER_GCC
#elif defined ( __ghs__ )
    /**
    * @brief Symbol required to be defined when GREENHILLS compiler is used.
    */
    #define COMPILER_GHS
#elif defined ( __ICCARM__ )
    /**
    * @brief Symbol required to be defined when IAR compiler is used.
    */
    #define COMPILER_IAR
#endif

#if defined (COMPILER_IAR)
#include <LowLevelIOInterface.h>

#pragma module_name = "?__write"
#endif

#if defined(COMPILER_GCC)
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#endif /* COMPILER_GCC */


#define DEBUG_INSTANCE           1UL
#define DEBUG_UART               UART1              /* define uart1 for debug output  */
#define DEBUG_UART_IRQ           UART1_IRQn         /* define uart1 interrupt vector  */
#define DEBUG_UART_CLK           CLK_UART1          /* define uart1 ckgen clock value */
#define DEBUG_UART_SRST          SRST_UART1         /* define uart1 ckgen reset value */
#define DEBUG_UART_BAUDRATE      115200UL
#if 0
#define DEBUG_UART_TX_PIN        PORTB, 22UL        /* define uart1 tx gpio */
#define DEBUG_UART_RX_PIN        PORTB, 23UL        /* define uart1 rx gpio */
#define DEBUG_UART_TX_FUNC       PORT_MUX_ALT5      /* define uart1 tx pinmux */
#define DEBUG_UART_RX_FUNC       PORT_MUX_ALT3      /* define uart1 rx pinmux */
#else
#define DEBUG_UART_TX_PIN        PORTC, 9UL         /* define uart1 tx gpio */
#define DEBUG_UART_RX_PIN        PORTC, 8UL         /* define uart1 rx gpio */
#define DEBUG_UART_TX_FUNC       PORT_MUX_ALT2      /* define uart1 tx pinmux */
#define DEBUG_UART_RX_FUNC       PORT_MUX_ALT2      /* define uart1 rx pinmux */
#endif

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* debug init flag */
static uint8_t s_debugInit = 0U;

#ifndef COMPILER_ARM_CLANG
/* define std FILE struct */
struct __FILE
{
    int handle;
};
/* refine stdout,stdin,stderr */
FILE __stdout;
FILE __stdin;
FILE __stderr;
#endif

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
static int uart_putc(int ch)
{
    if (s_debugInit != 0U)
    {
        while (!UART_GetStatusFlag(DEBUG_UART, UART_TX_DATA_NOT_FULL));
        UART_Putchar(DEBUG_UART, (uint8_t)ch);
    }

    return ch;
}

static int uart_getc(void)
{
    int ch = -1;

    if (s_debugInit != 0U)
    {
        if (UART_GetStatusFlag(DEBUG_UART, UART_RX_DATA_READY))
        {
            UART_Getchar(DEBUG_UART, (uint8_t *)&ch);
            ch &= 0xFF;
        }
    }

    return ch;
}

#if defined(COMPILER_ARM_CC) || defined(COMPILER_ARM_CLANG)
#if defined(COMPILER_ARM_CC) || defined(__MICROLIB)
/*!
 * @brief Refine _sys_exit.
 *
 * @param[in] x: no use
 * @return always 0
 */
int _sys_exit(int x)
{
    x = x;
    (void)x;

    return 0;
}

/*!
 * @brief Refine _ttywrch.
 *
 * @param[in] x: no use
 * @return always 0
 */
int _ttywrch(int x)
{
    x = x;
    (void)x;

    return 0;
}

#ifdef ITM_DEBUG_OUT
/*!
 * @brief Put a char to ITM.
 *
 * @param[in] ch: the char to put
 * @param[in] f:  file pointer for the std input
 * @return return the char of be put
 */
int fputc(int ch, FILE *f)
{
    if (s_debugInit != 0U)
    {
        ITM_SendChar(ch);
    }

    return ch;
}

/* for ITM use */
volatile int ITM_RxBuffer;

/*!
 * @brief Get a char from ITM.
 *
 * @param[in] f: file pointer for the std input
 * @return -1: not get char, other: the char get from ITM
 */
int fgetc(FILE *f)
{
    int ch = -1;

    if (s_debugInit != 0U)
    {
        ch = ITM_ReceiveChar();
    }

    return ch;
}
#else
/*!
 * @brief Put a char to UART.
 *
 * @param[in] ch: the char to put
 * @param[in] f:  file pointer for the std input
 * @return return the char of be put
 */
int fputc(int ch, FILE *f)
{
    (void)f;

    return uart_putc(ch);
}

/*!
 * @brief Get a char from UART.
 *
 * @param[in] f: file pointer for the std input
 * @return -1: not get char, other: the char get from UART
 */
int fgetc(FILE *f)
{
    (void)f;

    return uart_getc();
}
#endif /* ITM_DEBUG_OUT */
#else
__attribute__((weak))
int _sys_open (const char *name, int openmode)
{
  (void)name;
  (void)openmode;

  return 0;
}

__attribute__((weak))
int _sys_close (int fh)
{
  (void)fh;
  return 0;
}

__attribute__((weak))
int _sys_write (int fh, const uint8_t *buf, uint32_t len, int mode)
{
  int ch;
  uint32_t i;
  (void)mode;
  (void)fh;

  for (i = 0U; i < len; i++)
  {
    ch = (int)buf[i];
    uart_putc(ch);
  }

  return (0);
}

__attribute__((weak))
int _sys_read (int fh, uint8_t *buf, uint32_t len, int mode)
{
  (void)buf;
  (void)len;
  (void)mode;
  (void)fh;
  return uart_getc();
}

__attribute__((weak))
int _sys_istty (int fh)
{
  (void)fh;
  return (0);
}

__attribute__((weak))
int _sys_seek (int fh, long pos)
{
  (void)pos;
  (void)fh;
  return (-1);
}

__attribute__((weak))
long _sys_flen (int fh)
{
  (void)fh;
  return (0);
}
#endif /* __MICROLIB */
#endif /* #if defined(COMPILER_ARM_CC) || defined(COMPILER_ARM_CLANG) */

/*!
 * @brief Set the debug out is invalid.
 *
 * @param[in] none
 * @return none
 */
void DeinitDebug(void)
{
    s_debugInit = 0U;
}

/*!
 * @brief Init debug out, and set the debug out is valid.
 *
 * @param[in] none
 * @return none
 */
void InitDebug(void)
{
#ifndef ITM_DEBUG_OUT
#ifdef DEBUG_CMD_INTERRUPT
    NVIC_SetPriority(DEBUG_UART_IRQ, 3U);
    NVIC_ClearPendingIRQ(DEBUG_UART_IRQ);
    NVIC_EnableIRQ(DEBUG_UART_IRQ);
#endif /* DEBUG_CMD_INTERRUPT */

    /* Set pinmux */
    GPIO_DRV_SetMuxModeSel(DEBUG_UART_TX_PIN, DEBUG_UART_TX_FUNC);
    GPIO_DRV_SetMuxModeSel(DEBUG_UART_RX_PIN, DEBUG_UART_RX_FUNC);

    /* Check UART function clock */
    if (0U == (CKGEN->PERI_CLK_MUX2 & \
        (CKGEN_PERI_CLK_MUX2_UART0_MUX_Msk << (DEBUG_INSTANCE * 4U))))
    {
        /* Default set clock to HSIDIV2 */
        CKGEN->PERI_CLK_MUX2 |= 5U << (DEBUG_INSTANCE * 4U);
    }
    /* Enable UART bus clock and release UART module from reset status */
    CKGEN_DRV_Enable(DEBUG_UART_CLK, true);
    CKGEN_DRV_SoftReset(DEBUG_UART_SRST, true);

    /* Set the baudrate */
    (void)UART_DRV_SetBaudRate(DEBUG_INSTANCE, DEBUG_UART_BAUDRATE);

    UART_SetBitCountPerChar(DEBUG_UART, UART_8_BITS_PER_CHAR);
    UART_SetStopBitCount(DEBUG_UART, UART_ONE_STOP_BIT);
    UART_SetParityMode(DEBUG_UART, UART_PARITY_DISABLED);
    UART_SetFIFO(DEBUG_UART, true);

    /* Enable UART TX and RX */
    UART_SetTransmitterCmd(DEBUG_UART, true);
    UART_SetReceiverCmd(DEBUG_UART, true);
#endif /* ITM_DEBUG_OUT */
    s_debugInit = 1U;
}

#if defined(COMPILER_IAR)
int putchar(int ch)
{
    return uart_putc(ch);;
}

int getchar(void)
{
    return uart_getc();
}
#endif /* COMPILER_IAR */

#if defined(COMPILER_GCC)

int __io_putchar(int ch)
{
    return uart_putc(ch);
}

int __io_getchar(void)
{
    return uart_getc();
}

int _write(int file, char *Ptr, int len)
{
    (void)file;

    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
        (void)__io_putchar(*Ptr++);
    }

    return len;
}

int _read(int file, char *Ptr, int len)
{
    (void)file;

    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
        *Ptr++ = __io_getchar();
    }

    return len;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    return -1;
}
#endif /* COMPILER_GCC */

#if defined(COMPILER_GHS)
int write(int fd, char *Ptr, int len)
{
    (void) fd;

    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
        (void)uart_putc(*Ptr++);
    }

    return len;
}

int read(int fd, char *Ptr, int len)
{
    (void) fd;

    for (int DataIdx = 0; DataIdx < len; DataIdx++)
    {
        *Ptr++ = uart_getc();
    }

    return len;
}
#endif /* COMPILER_GHS */

/*!
 * @brief Printf log to uart.
 * @return void
 */
void Debug_Printf(const char * format, ...)
{
#if ATC_DEBUG_OUT_INFO
    va_list list;
    uint8_t len, i;
    /* store send message */
    uint8_t s_logMessage[MAX_DEBUG_BUFF_SIZE] = {0};

    if (format != NULL)
    {
        va_start(list, format);
        len = vsnprintf((char *)&s_logMessage[0], MAX_DEBUG_BUFF_SIZE, format, list);

        for (i = 0; i < len; i++)
        {
            (void)uart_putc(s_logMessage[i]);
        }

        va_end(list);
    }
#endif
}
/* =============================================  EOF  ============================================== */
