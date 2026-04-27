
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
