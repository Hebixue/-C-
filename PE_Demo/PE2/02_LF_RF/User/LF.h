#ifndef LF_H_
#define LF_H_
#include <stdint.h>

// Define main_state enum
#define IDLE 0
#define LF_WAKEUP 1
#define LF_WAKEUP_LOOP 2
#define IMMO 3
#define IMMO_LOOP 4
#define DIAG 5
#define WAIT4DH 6
    typedef struct
    {
        uint8_t Tx[240];
        uint8_t Rx[240];
        uint8_t WorkingBuffer[128];
        uint8_t Length;
    } SPI_BuffersTypeDef;

void SendLFWakeUp(void);

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

#endif /*LF_H_*/
