# LF/RF/CAN/RSSI 当前工作交接说明（面向 AI 助手）

## 工程位置

- 工作目录：`C:\Users\ALCA\Desktop\CCCC\software`
- 当前主要工程：`PE_Demo\PE2\02_LF_RF`
- 当前策略：保留 `02_LF_RF_原版` 的 LF/RF 主链路，在其基础上加入 CAN、继电器、三类 CAN 报文、RSSI 去重与缓存。
- 当前已经把 `03_Handle` 中 LED1 对应的门把手 DH0 触发逻辑最小移植到 `02_LF_RF`，并将 LF 从一直轮询改为“门把手触发后限时轮询”。

## 当前主循环行为

文件：`PE_Demo\PE2\02_LF_RF\User\main.c`

当前主循环核心逻辑：

1. `CAN_App_Task()` 持续跑，用于从 CAN 应用队列里取帧并发送。
2. 空闲时不再持续 LF 轮询，而是等待门把手 DH0 事件。
3. 检测到门把手 DH0 后：
   - `LED1_ON`；
   - 启动约 `1.5s` 的 LF 轮询窗口；
   - LF 轮询窗口内每 `85ms` 调用一次 `SendLFWakeUp()`；
   - 每次 LF 发送后发送一帧 `0x301`，状态为 `PKES_SYS_LF_WAKEUP`；
   - 天线编号 `active_antenna` 按 `1,2,3,4` 循环；
   - 调用 `PKES_Core_UpdateTick()`。
4. LF 轮询窗口结束后：
   - 停止 LF 定时器；
   - `LED1_OFF`；
   - 将 `NRES` 拉低，再拉高；
   - 重新执行门把手 DH 初始化。
5. RF 收到完整 5 字节后：
   - 从 `gRf_Buf[0..4]` 复制到本地 `buf[5]`；
   - 清 `gRFFull`；
   - 调用 `PKES_Core_ProcessRFData(buf)`。

## 门把手触发 LF 轮询

当前已将 `PE_Demo\PE2\03_Handle` 中 LED1 对应的门把手 DH0 逻辑移植到 `PE_Demo\PE2\02_LF_RF\User\main.c`。

注意：本轮移植按用户要求做最小改动，没有加入额外 IRQ 锁存、完整 rearm 状态机或复杂恢复逻辑。

当前新增关键函数：

```c
static void DH_Init(void);
static uint8_t DH_GetEvent(void);
static void DH_ClearEvent(void);
static void DH_StartLFWindow(void);
static void DH_StopLFWindow(void);
```

门把手触发条件：

```c
if ((GPIO_DRV_ReadPins(IRQ_GPIO) & (1u << IRQ_PIN)) != 0u)
{
    uint8_t event = DH_GetEvent();
    if ((event & 0x01u) != 0u)
    {
        LED1_ON;
        DH_StartLFWindow();
    }
}
```

LF 窗口长度：

```c
#define HANDLE_LF_POLL_TICKS 18u
```

因为 LF 定时器周期是 `85ms`：

```text
18 * 85ms = 1530ms
```

即约 `1.5s`。

### NRES 时序

用户特别强调：`NRES` 是低频天线驱动和门把手功能的初始化关键脚。当前代码按如下方式处理：

1. 上电初始化时：

```text
NRES = 1
DH_Init()
DH_ClearEvent()
RFS/checkStatus
```

2. 门把手触发并准备进入 LF 轮询时：

```text
停止 LF 定时器
NRES = 0
NRES = 1
启动 1.5s LF 轮询窗口
立即置 LF_timerOutFlag = 1，马上发第一帧 LF
```

3. LF 轮询窗口结束时：

```text
停止 LF 定时器
NRES = 0
NRES = 1
DH_Init()
DH_ClearEvent()
RFS/checkStatus
```

这样做的目的：从 DH 检测链路切到 LF 发送链路前，先通过 `NRES` 复位释放 ATA5293 状态；LF 发送窗口结束后再复位并重新进入门把手检测。

## LF 轮询周期

文件：`PE_Demo\PE2\02_LF_RF\User\timer.c`

当前 LF 轮询周期已改为：

```c
timerConfig.period = 85000U;
```

单位是微秒，即：

```text
85 ms / 次 LF
```

当前 LF 不是一直运行，而是在门把手触发后的 1.5s 窗口内轮询 4 根天线：

```text
天线1 -> 天线2 -> 天线3 -> 天线4 -> ...
```

因此单根天线有效轮询周期为：

```text
85ms * 4 = 340ms
```

如果只接入一根天线，且处于 LF 轮询窗口内，理论上该天线的有效 RSSI 上报周期约为 `340ms`，即 `2.9 帧/s`。

## LF 发送函数

文件：`PE_Demo\PE2\02_LF_RF\User\LF.c`

关键函数：

```c
void SendLFWakeUp(void)
```

关键点：

- 按 `active_antenna` 选择不同天线；
- LF 数据区长度是 `22 * 8 = 176 bit`；
- 当前 TX 编码/速率配置：

```c
lf_ata5293_SWRI(TXENC, (TX_NRZ | TX_M4kBd));
```

从代码推算 LF 数据段时间约：

```text
176 bit / 4 kbps = 44 ms
```

注意：这个 44ms 不是完整 LF 发射总时间，`PTX/SC/SPI/状态切换` 还会增加额外时间。

## RF 帧格式

当前被动 RSSI RF 帧为 5 字节：

```text
Byte[0] Antenna ID: 0xA2~0xA5
Byte[1] RSSI_L
Byte[2] RSSI_H
Byte[3] CRC_L
Byte[4] CRC_H
```

解析规则：

```c
uint8_t ant_id = buf[0];
uint16_t rssi = ((uint16_t)buf[2] << 8) | buf[1];
uint16_t crc_rx = (uint16_t)buf[3] | ((uint16_t)buf[4] << 8);
uint16_t crc_calc = crc16_ccitt_false(buf, 3);
```

天线映射：

```text
0xA2 -> 天线1
0xA3 -> 天线2
0xA4 -> 天线3
0xA5 -> 天线4
```

代码里通过：

```c
ant_idx = ant_id - 0xA1;
```

得到 `1~4` 的天线编号。

## CAN 报文定义

文件：`PE_Demo\PE2\02_LF_RF\User\pkes_can_protocol.h`

当前三类标准帧：

```c
#define PKES_CAN_ID_SYS_STATUS 0x301u
#define PKES_CAN_ID_KEY_RSSI   0x302u
#define PKES_CAN_ID_DISTANCE   0x303u
```

### 0x301 系统状态帧

`0x301` Byte0 是循环计数：

```c
frame.seq = s_can_seq++;
```

每调用一次 `CAN_App_SendSysState()`，Byte0 自增。

当前 `0x301` 主要来源：

- 门把手触发后的 1.5s LF 窗口内，每次 LF 轮询后发送 `PKES_SYS_LF_WAKEUP`，约 `11.8 帧/s`；
- 每收到一条去重后的有效 RSSI 后发送 `PKES_SYS_RF_RECEIVED`，单天线约 `2~3 帧/s`；
- 主动开锁/闭锁时发送执行结果；
- 初始化/心跳状态也可能发送。

因此只接一根天线并触发门把手后，当前实测 `0x301` 约 `13~15 帧/s` 是合理的。

### 0x301 常见帧解释

门把手触发 LF 窗口时，常见 `0x301` 数据如：

```text
2F 02 01 00 03 03 01 00
30 02 01 00 04 03 01 00
31 02 01 00 01 03 01 00
32 02 01 00 02 03 01 00
```

含义：

```text
Byte0: 计数递增
Byte1: 02 = PKES_SYS_LF_WAKEUP
Byte2: 01 = PKES_STATUS_LF_WAKEUP_OK
Byte3: 00 = 区域未知
Byte4: 当前 LF 轮询天线编号
Byte5: 03 = 门锁保持不变
Byte6: 01 = PKES_TRIGGER_HANDLE，触发源为门把手
Byte7: 00 = 无额外标志
```

收到有效 RSSI 时，常见 `0x301` 数据如：

```text
33 04 03 00 02 03 00 06
```

含义：

```text
Byte1: 04 = PKES_SYS_RF_RECEIVED
Byte2: 03 = PKES_STATUS_FRAME_VALID
Byte4: RSSI 对应天线编号
Byte7: 06 = PKES_FLAG_CRC_OK | PKES_FLAG_RSSI_VALID
```

### 0x302 钥匙信息与 RSSI 帧

当前格式：

```text
Byte0 KeyID
Byte1 RSSI_L
Byte2 RSSI_H
Byte3 RSSI_Ant
Byte4 Reserved = 0
Byte5 Reserved = 0
Byte6 Reserved = 0
Byte7 Reserved = 0
```

当前 `KeyID` 固定为：

```c
#define PKES_CORE_KEY_ID 0x00000084u
```

注意：`0x302` 没有计数字段，也没有 flags 字段。

只接天线4时，正常 `0x302` 数据形态类似：

```text
84 xx xx 04 00 00 00 00
```

其中 `xx xx` 是 RSSI 小端值。

### 0x303 测距与区域结果帧

目前定义已存在，但当前还没有真正做 RSSI -> 距离换算、滤波、区域判断。

## CAN 发送机制

文件：`PE_Demo\PE2\02_LF_RF\User\pkes_can_app.c`

当前重复发送已关闭：

```c
#define CAN_APP_REPEAT_COUNT 1u
```

这个值的含义是“总共发送 1 次”，不是“额外发送 1 次”。

发送机制：

1. `CAN_App_SendSysState()` / `CAN_App_SendKeyRssi()` / `CAN_App_SendDistance()` 只负责打包并入队。
2. `CAN_App_Task()` 从队列取帧，并调用 `CAN_SendData()`。
3. `CAN_SendData()` 调用底层 `CAN_DRV_Send()`。

当前 CAN 波特率：

```c
CAN_BITRATE_500K
```

标准 8 字节数据帧总线占用约 `0.2~0.3ms/帧`。

## 官方确认的 RF 重复包行为

用户已询问官方，官方确认：

```text
遥控器 RF 会连续发送 3 次相同的 RSSI 值，这是写死的防丢包机制。
```

之前现象：

- `0x302` 每次成对或三连出现；
- 时间间隔如 `30ms/31ms/61ms`；
- 这不是 CAN 应用层重复发送导致的。

## RSSI 去重与缓存

文件：`PE_Demo\PE2\02_LF_RF\User\pkes_core.c`

当前已加入：

```c
#define PKES_CORE_RSSI_SAMPLE_DEPTH 5u
#define PKES_CORE_RSSI_DUP_WINDOW_MS 80u
```

每根天线有独立 RSSI 环形缓存：

```c
typedef struct
{
    uint16_t rssi;
    uint32_t timestamp_ms;
    uint8_t valid;
} pkes_core_rssi_sample_t;

typedef struct
{
    pkes_core_rssi_sample_t samples[PKES_CORE_RSSI_SAMPLE_DEPTH];
    uint8_t head;
    uint8_t count;
    uint16_t last_rssi;
    uint32_t last_time_ms;
    uint8_t last_valid;
    uint8_t duplicate_count;
} pkes_core_antenna_rssi_t;
```

当前行为：

1. RF CRC 校验通过；
2. 判断 `buf[0]` 是否为 `0xA2~0xA5`；
3. 得到天线编号和 RSSI；
4. 如果 `80ms` 内同一天线、同 RSSI 再次出现：
   - 判定为遥控器重复包；
   - 不写入 RSSI 样本缓存；
   - 不发送 `0x302`；
   - 不发送 `RF_RECEIVED` 的 `0x301`；
5. 如果不是重复包：
   - 写入对应天线的 5 深度环形缓存；
   - 发送 `0x302`；
   - 发送 `0x301 RF_RECEIVED`。

重要：当前没有做滤波、均值、中值、距离换算或区域判断。  
当前只是保存“去重后的原始 RSSI 样本”。

## 当前验证结果

用户实测当前结果：

- 只接一根天线；
- 门把手触发后，天线启动；
- 触发后 `0x301` 会连续发送一串 `LF_WAKEUP` 状态帧；
- `0x302` 帧率约 `2~3 帧/s`；
- `0x302` 每次发送间隔约 `340ms`；
- `0x301` 帧率约 `13~15 帧/s`。

这与当前逻辑一致：

```text
0x302: 85ms * 4根天线 = 340ms/次
0x301: LF_WAKEUP约11.8帧/s + RF_RECEIVED约2~3帧/s = 约14~15帧/s
```

## 后续建议

后续处理建议基于当前 RSSI 缓存继续做，不要再把遥控器 3 连发当成 3 个有效测距样本。

推荐处理顺序：

1. 先从每根天线缓存中取最近 3 个有效 RSSI；
2. 做中值滤波；
3. 再考虑一阶低通或滑动均值；
4. 根据实测标定表或拟合函数做 RSSI -> 距离；
5. 生成 `0x303` 距离与区域结果帧；
6. 决定是否立即开锁。

建议保留策略：

```text
遥控器重复包：去重，只用于确认 RF 活动
去重后的 RSSI：保存为有效样本
滤波/距离/区域判断：后续单独实现
```

## 注意事项

- 不要把 `CAN_APP_REPEAT_COUNT = 1u` 理解为额外发送 1 次，它表示总共发送 1 次。
- 不要把 `0x302` 的 Byte1/Byte2 当成计数，它们是 RSSI 小端值。
- `0x301` 的 Byte0 是计数，跳变可能来自 CAN 软件只显示同 ID 最新帧，也可能因为实际帧率较高。
- 当前 LF 已改为门把手触发后的限时轮询，不再是一直定时轮询。
- 门把手触发进入 LF 前和 LF 完成回到 DH 前，都处理了 `NRES` 拉低/拉高。
- 本轮门把手移植按用户要求没有加 IRQ 锁存和完整 rearm；后续若出现多次触发失效，再考虑恢复完整状态机。
- 如果只接一根天线但 `0x302` 明显高于 3 帧/s，应优先检查 RF 重复包去重是否失效，或 CAN 软件是否统计重复显示。
