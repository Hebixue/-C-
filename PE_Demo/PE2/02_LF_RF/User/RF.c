#include "RF.h"
#include "gpio.h"
#include <stdbool.h>
// 对外变量定义
volatile uint8_t gRFFull = 0;
volatile uint8_t gRf_Buf[25] = {0};
volatile uint8_t gRFWakeup = 0;

typedef enum
{
	RF_ST_IDLE = 0, // 等待稳定静默
	RF_ST_WAKE = 1, // 采样判唤醒码 0xCCCC
	RF_ST_SYNC = 2, // 等待 4ms 固定电平
	RF_ST_DATA = 3	// 采样判数据
} rf_state_t;

static inline uint8_t rf_read(void)
{
	uint8_t b = (uint8_t)RF_READ_PIN();
#if RF_INVERT
	b ^= 1u;
#endif
	return b;
}

static inline void buf_clear(void)
{
	for (int i = 0; i < 25; i++)
		gRf_Buf[i] = 0;
}

void RF_Reset(void)
{
	gRFFull = 0;
	gRFWakeup = 0;
	buf_clear();
}

void LD_RF_Recive(void)
{
	// 读取当前采样点
	uint8_t s = rf_read();

	// 状态机变量
	static rf_state_t st = RF_ST_IDLE;

	// 静默检测（减少30/200干扰影响）
	static uint8_t last = 0;
	static uint16_t stable_cnt = 0;

	// 通用：bit内投票
	static uint8_t bit_sample_cnt = 0;
	static uint8_t ones_cnt = 0;

	// 唤醒移位寄存器
	static uint16_t wake_shift = 0;
	static uint8_t wake_got_bits = 0;

	// 同步电平计数
	static uint16_t sync_cnt = 0;

	// 数据计数/打包
	static uint16_t data_bit_cnt = 0;
	static uint8_t byte_idx = 0;

	// 静默计数更新
	if (s == last)
	{
		if (stable_cnt < 0xFFFF)
			stable_cnt++;
	}
	else
	{
		stable_cnt = 0;
		last = s;
	}

	switch (st)
	{
	default:
	case RF_ST_IDLE:
		// 要求至少 PRE_SILENCE_CNT 个采样点保持同一电平后，开始进入 WAKE
		if (stable_cnt >= PRE_SILENCE_CNT)
		{
			// 初始化唤醒采样器
			bit_sample_cnt = 0;
			ones_cnt = 0;
			wake_shift = 0;
			wake_got_bits = 0;
			gRFWakeup = 0;

			st = RF_ST_WAKE;
		}
		break;

	case RF_ST_WAKE:
		// 每个bit=20点：累计ones
		ones_cnt += s;
		bit_sample_cnt++;

		if (bit_sample_cnt >= NRZ_SAMPLES_PER_BIT)
		{
			// 多数投票出一位
			uint8_t bit = (ones_cnt > (NRZ_SAMPLES_PER_BIT / 2)) ? 1u : 0u;

			wake_shift = (uint16_t)((wake_shift << 1) | bit);
			if (wake_got_bits < WAKE_BITS)
				wake_got_bits++;

			// 重置下一bit
			bit_sample_cnt = 0;
			ones_cnt = 0;

			// 已累积16bit后做滑动匹配
			if (wake_got_bits >= WAKE_BITS)
			{
				if (wake_shift == WAKE_PATTERN)
				{
					gRFWakeup = 1;

					// 进入同步段
					st = RF_ST_SYNC;
					sync_cnt = 0;
				}
				// 否则继续滑动：保持 wake_got_bits=16 不清零，持续移位
			}
		}
		break;

	case RF_ST_SYNC:
		// 统计连续 SYNC_LEVEL
		if (s == SYNC_LEVEL)
		{
			if (sync_cnt < 0xFFFF)
				sync_cnt++;
		}
		else
		{
			// 同步段结束的边沿出现了：检查长度是否在窗口内
			uint16_t minc = (SYNC_CNT > SYNC_TOL_CNT) ? (SYNC_CNT - SYNC_TOL_CNT) : 0;
			uint16_t maxc = (SYNC_CNT + SYNC_TOL_CNT);

			if (sync_cnt >= minc && sync_cnt <= maxc)
			{
				// 同步成立 -> 进入数据
				st = RF_ST_DATA;

				// 初始化数据采样器
				bit_sample_cnt = 0;
				ones_cnt = 0;
				data_bit_cnt = 0;
				byte_idx = 0;
				buf_clear();
				gRFFull = 0;
			}
			else
			{
				// 不像同步段，回到IDLE重新来
				st = RF_ST_IDLE;
			}
			sync_cnt = 0;
		}

		// 如果一直不结束，也可以加上上限保护（可选）
		if (sync_cnt > (SYNC_CNT + SYNC_TOL_CNT + 200))
		{
			st = RF_ST_IDLE;
			sync_cnt = 0;
		}
		break;

	case RF_ST_DATA:
		// 采样一个bit的20点，投票出0/1
		ones_cnt += s;
		bit_sample_cnt++;

		if (bit_sample_cnt >= NRZ_SAMPLES_PER_BIT)
		{
			uint8_t bit = (ones_cnt > (NRZ_SAMPLES_PER_BIT / 2)) ? 1u : 0u;

			// MSB-first 打包：直接写入当前 bit 对应位置，避免连续左移累积。
			{
				uint8_t bit_pos = (uint8_t)(7u - (data_bit_cnt & 7u));
				uint8_t bit_mask = (uint8_t)(1u << bit_pos);

				if (bit != 0u)
				{
					gRf_Buf[byte_idx] = (uint8_t)(gRf_Buf[byte_idx] | bit_mask);
				}
				else
				{
					gRf_Buf[byte_idx] = (uint8_t)(gRf_Buf[byte_idx] & (uint8_t)(~bit_mask));
				}
			}
			data_bit_cnt++;

			if ((data_bit_cnt & 7u) == 0u)
			{
				byte_idx++;
				if (byte_idx >= 13u)
				{
					// 防越界：直接认为满帧
					gRFFull = 1;
					st = RF_ST_IDLE;
					break;
				}
			}

			// 下一bit
			bit_sample_cnt = 0;
			ones_cnt = 0;

			if (data_bit_cnt >= DATA_NBIT)
			{
				gRFFull = 1;
				st = RF_ST_IDLE;
			}
		}

		break;
	}
}
