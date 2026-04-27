
#ifndef RF_H_
#define RF_H_

#include <string.h>
#include <stdint.h>

//==============================================================================
//									初始化
//==============================================================================
#ifndef RF_READ_PIN
#define RF_READ_PIN() (((GPIO_DRV_ReadPins(RFDATA_GPIO) & (1u << RFDATA_PIN)) != 0u) ? 1u : 0u)
#endif

//===================== 可调参数 =====================

// 采样周期（由定时器保证 25us）
#define SAMPLE_US 25u

// NRZ 码率 4kbps => bit=250us => 10点/bit
// 从2kbps(500us/bit,20点)升到4kbps(250us/bit,10点)
// 投票门限 NRZ_SAMPLES_PER_BIT/2=5，过半数机制不变，抗噪性仍OK
// 单帧时间：68ms→38ms，3帧总计：204ms→114ms
// 对应遥控端 RKE.c 需同步将 RF 波特率改为 4kbps
#define NRZ_BIT_US 250u
#define NRZ_SAMPLES_PER_BIT (NRZ_BIT_US / SAMPLE_US) // 10

// 输入极性（如发现整体反相就置 1）
#define RF_INVERT 0u

// 前导干扰：30us高+200us低（可选处理：需要静默后再启动解码）
#define PRE_SILENCE_US 150u                          // 要求至少 0.5ms 的稳定电平再开始找唤醒（建议）
#define PRE_SILENCE_CNT (PRE_SILENCE_US / SAMPLE_US) // 20

// 唤醒码：1100110011001100
#define WAKE_BITS 16u
#define WAKE_PATTERN 0xCCCCu

// 同步固定电平：4ms（你说是 4ms 电平，通常是低；若是高改 SYNC_LEVEL）
#define SYNC_US 4000u
#define SYNC_CNT (SYNC_US / SAMPLE_US) // 160
#define SYNC_TOL_CNT 20u               // 容差（±1ms）可调
#define SYNC_LEVEL 0u                  // 0=低电平同步；1=高电平同步

// 数据长度（bit数），你按实际帧改
#define DATA_NBIT 104u

//===================== 对外变量 =====================
extern volatile uint8_t gRFFull;     // 一帧收满
extern volatile uint8_t gRf_Buf[25]; // 缓冲
extern volatile uint8_t gRFWakeup;   // 唤醒码命中（可选）

//===================== API =====================
void LD_RF_Recive(void); // 每 25us 调用一次

// void RF_Reset(void);
//==============================================================================
//									end
//==============================================================================

#endif /* RF_H_ */
