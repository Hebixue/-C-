#ifndef PKES_CAN_PROTOCOL_H_
#define PKES_CAN_PROTOCOL_H_

#include <stdint.h>

/* PKES 所有 CAN 报文均使用标准 11 位 ID，数据区固定 8 字节。 */
#define PKES_CAN_DLC 8u

#define PKES_CAN_ID_SYS_STATUS 0x301u
#define PKES_CAN_ID_KEY_RSSI   0x302u
#define PKES_CAN_ID_DISTANCE   0x303u

/* 0x301 Byte1：系统状态码。 */
typedef enum
{
    PKES_SYS_STANDBY = 0x00u,
    PKES_SYS_TRIGGER_DETECTED = 0x01u,
    PKES_SYS_LF_WAKEUP = 0x02u,
    PKES_SYS_WAIT_RF = 0x03u,
    PKES_SYS_RF_RECEIVED = 0x04u,
    PKES_SYS_ID_AUTH_OK = 0x05u,
    PKES_SYS_ID_AUTH_FAIL = 0x06u,
    PKES_SYS_RSSI_PROCESS = 0x07u,
    PKES_SYS_SINGLE_DISTANCE_DONE = 0x08u,
    PKES_SYS_REGION_DONE = 0x09u,
    PKES_SYS_UNLOCK_DONE = 0x0Au,
    PKES_SYS_LOCK_DONE = 0x0Bu,
    PKES_SYS_TIMEOUT = 0x0Cu,
    PKES_SYS_ERROR = 0x0Du
} pkes_sys_state_t;

/* 0x301 Byte2：状态/结果码。 */
typedef enum
{
    PKES_STATUS_NONE = 0x00u,
    PKES_STATUS_LF_WAKEUP_OK = 0x01u,
    PKES_STATUS_LF_WAKEUP_FAIL = 0x02u,
    PKES_STATUS_FRAME_VALID = 0x03u,
    PKES_STATUS_CRC_ERROR = 0x04u,
    PKES_STATUS_UNKNOWN_ID = 0x05u,
    PKES_STATUS_UNLOCK_CMD = 0x06u,
    PKES_STATUS_LOCK_CMD = 0x07u,
    PKES_STATUS_DISTANCE_VALID = 0x08u,
    PKES_STATUS_REGION_VALID = 0x09u,
    PKES_STATUS_REGION_INVALID = 0x0Au,
    PKES_STATUS_UNLOCK_DONE = 0x0Bu,
    PKES_STATUS_LOCK_DONE = 0x0Cu,
    PKES_STATUS_RELAY_ERROR = 0x0Du,
    PKES_STATUS_DISTANCE_OUT_OF_RANGE = 0x0Eu
} pkes_status_code_t;

/* 0x301 Byte3 / 0x303 Byte3：区域判断结果。 */
typedef enum
{
    PKES_REGION_UNKNOWN = 0x00u,
    PKES_REGION_LEFT_DOOR = 0x01u,
    PKES_REGION_RIGHT_DOOR = 0x02u,
    PKES_REGION_TRUNK = 0x03u,
    PKES_REGION_INSIDE = 0x04u,
    PKES_REGION_FAR = 0x05u,
    PKES_REGION_BOUNDARY = 0x06u
} pkes_region_code_t;

/* 0x301 Byte5：门锁状态。 */
typedef enum
{
    PKES_LOCK_UNKNOWN = 0x00u,
    PKES_LOCK_LOCKED = 0x01u,
    PKES_LOCK_UNLOCKED = 0x02u,
    PKES_LOCK_KEEP = 0x03u
} pkes_lock_state_t;

/* 0x301 Byte6：触发源。 */
typedef enum
{
    PKES_TRIGGER_NONE = 0x00u,
    PKES_TRIGGER_HANDLE = 0x01u,
    PKES_TRIGGER_KEY1 = 0x02u,
    PKES_TRIGGER_KEY2 = 0x03u,
    PKES_TRIGGER_RF_UNLOCK = 0x04u,
    PKES_TRIGGER_RF_LOCK = 0x05u
} pkes_trigger_src_t;

/* 0x301 Byte7：标志位。 */
#define PKES_FLAG_ID_OK         (1u << 0)
#define PKES_FLAG_CRC_OK        (1u << 1)
#define PKES_FLAG_RSSI_VALID    (1u << 2)
#define PKES_FLAG_DISTANCE_VALID (1u << 3)
#define PKES_FLAG_REGION_VALID  (1u << 4)

/* 0x301：系统状态与执行结果帧。 */
typedef struct
{
    /* Byte0：循环递增报文计数。 */
    uint8_t seq;
    /* Byte1：pkes_sys_state_t 系统状态码。 */
    uint8_t sys_state;
    /* Byte2：pkes_status_code_t 状态/结果码。 */
    uint8_t status_code;
    /* Byte3：pkes_region_code_t 区域编码。 */
    uint8_t region_code;
    /* Byte4：当前触发或参与测量的天线编号，使用代码编号 1~4。 */
    uint8_t antenna_id;
    /* Byte5：pkes_lock_state_t 门锁状态。 */
    uint8_t lock_state;
    /* Byte6：pkes_trigger_src_t 触发源。 */
    uint8_t trigger_src;
    /* Byte7：PKES_FLAG_* 标志位。 */
    uint8_t flags;
} pkes_can_sys_status_t;

/* 0x302：钥匙信息与 RSSI 帧。 */
typedef struct
{
    /* Byte0：钥匙 ID。 */
    uint8_t key_id;
    /* Byte1~Byte2：原始 RSSI，小端序。 */
    uint8_t rssi_l;
    uint8_t rssi_h;
    /* Byte3：RSSI 对应天线编号，使用代码编号 1~4。 */
    uint8_t rssi_ant;
    /* Byte4~Byte7：保留字段，置 0。 */
    uint8_t reserved0;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
} pkes_can_key_rssi_t;

/* 0x303：测距与区域结果帧。 */
typedef struct
{
    /* Byte0：当前测距对应天线编号，使用代码编号 1~4。 */
    uint8_t antenna_id;
    /* Byte1~Byte2：距离值，Distance_cm = (Byte1 << 8) | Byte2。 */
    uint8_t dist_h;
    uint8_t dist_l;
    /* Byte3：pkes_region_code_t 区域编码。 */
    uint8_t region_code;
    /* Byte4：置信度，范围 0~100。 */
    uint8_t confidence;
    /* Byte5：本次结果使用的样本数量。 */
    uint8_t sample_cnt;
    /* Byte6~Byte7：保留字段，置 0。 */
    uint8_t reserved0;
    uint8_t reserved1;
} pkes_can_distance_t;

/* 将 0x301 结构体打包为 8 字节 CAN 数据。 */
static inline void PKES_CAN_PackSysStatus(const pkes_can_sys_status_t *src, uint8_t data[PKES_CAN_DLC])
{
    data[0] = src->seq;
    data[1] = src->sys_state;
    data[2] = src->status_code;
    data[3] = src->region_code;
    data[4] = src->antenna_id;
    data[5] = src->lock_state;
    data[6] = src->trigger_src;
    data[7] = src->flags;
}

/* 将 0x302 结构体打包为 8 字节 CAN 数据。 */
static inline void PKES_CAN_PackKeyRssi(const pkes_can_key_rssi_t *src, uint8_t data[PKES_CAN_DLC])
{
    data[0] = src->key_id;
    data[1] = src->rssi_l;
    data[2] = src->rssi_h;
    data[3] = src->rssi_ant;
    data[4] = src->reserved0;
    data[5] = src->reserved1;
    data[6] = src->reserved2;
    data[7] = src->reserved3;
}

/* 将 0x303 结构体打包为 8 字节 CAN 数据。 */
static inline void PKES_CAN_PackDistance(const pkes_can_distance_t *src, uint8_t data[PKES_CAN_DLC])
{
    data[0] = src->antenna_id;
    data[1] = src->dist_h;
    data[2] = src->dist_l;
    data[3] = src->region_code;
    data[4] = src->confidence;
    data[5] = src->sample_cnt;
    data[6] = src->reserved0;
    data[7] = src->reserved1;
}

/* 从已打包的 0x303 CAN 数据中读取距离值，单位 cm。 */
static inline uint16_t PKES_CAN_GetDistanceCm(const uint8_t data[PKES_CAN_DLC])
{
    return (uint16_t)(((uint16_t)data[1] << 8) | data[2]);
}

/* 将 16 位厘米距离值拆分写入 0x303 距离高低字节。 */
static inline void PKES_CAN_SetDistanceCm(pkes_can_distance_t *dst, uint16_t dist_cm)
{
    dst->dist_h = (uint8_t)((dist_cm >> 8) & 0xFFu);
    dst->dist_l = (uint8_t)(dist_cm & 0xFFu);
}

#endif /* PKES_CAN_PROTOCOL_H_ */

