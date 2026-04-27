#ifndef MAIN_H_
#define MAIN_H_
#include <stdint.h>
// Define main_state enum
#define IDLE 0
#define LF_WAKEUP 1
#define LF_WAKEUP_LOOP 2
#define IMMO 3
#define IMMO_LOOP 4
#define DIAG 5
#define WAIT4DH 6

void SendLFWakeUp(void);
void Immobilizer(void);
uint8_t GetAscii(uint8_t data);
static uint16_t crc16_ccitt_false(const uint8_t *data, size_t len)
#endif /* MAIN_H_ */

    /* =============================================  EOF  ============================================== */
