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

#ifndef _SPI_H
#define _SPI_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ===========================================  Includes  =========================================== */
#include "ac7840x.h"

/* ===========================================   Define    ========================================== */
#define SPI_TIMEOUT 5000U

#define SPI_INSTANCE (0U)
#define SPI_IRQ (SPI0_IRQn)
#define PORT_SPI_SCK (PORTB)
#define PORT_SPI_NCS (PORTB)
#define PORT_SPI_SIN (PORTB)
#define PORT_SPI_SOUT (PORTB)
#define PIN_SPI_SCK (2U)
#define PIN_SPI_NCS (5U)
#define PIN_SPI_SIN (3U)
#define PIN_SPI_SOUT (4U)

    /* ===========================================  Typedef  ============================================ */

    /* ==========================================  Variables  =========================================== */

    /* ====================================  Functions declaration  ===================================== */
    void SPI_Master_Init(void);
    void SPI1_Exchange8bitBuffer(uint8_t *dataOut, uint8_t *dataIn, uint8_t bufLen, uint8_t returnedDataStart);

    /* ======================================  Functions define  ======================================== */

#ifdef __cplusplus
}
#endif

#endif /* _SPI_H */

/* =============================================  EOF  ============================================== */
