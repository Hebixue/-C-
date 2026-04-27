/*******************************************************************/

#ifndef __CAN_H__
#define __CAN_H__
#include "can_drv.h"
#include <stdint.h>
#ifdef __cplusplus
    extern "C"
{
#endif

    void CAN_Test(void);
    status_t CAN_SendData(uint8_t instance, uint32_t id, const uint8_t *data, uint8_t len);
    void CAN_Init(uint8_t instance);
#ifdef __cplusplus
}
#endif

#endif /*__CAN_H__*/
