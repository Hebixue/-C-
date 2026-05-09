# AC78406YGLA PKES LF/RF/CAN Demo

这是一个基于 AutoChips `AC78406YGLA` 的 PKES 无钥匙进入控制工程，主要实现门把手触发、四天线 LF 唤醒、RF 回包解析、RSSI 测距/区域判断、CAN 状态上报和门锁继电器控制。

## 工程路径

当前主要工程位于：

```text
PE_Demo/PE2/02_LF_RF
```

使用 Keil MDK 打开：

```text
PE_Demo/PE2/02_LF_RF/demo.uvprojx
```

## 核心流程

1. 门把手触发后，主控读取 ATA5293 门把手事件并识别左门/右门区域。
2. 系统启动 LF 轮询窗口，按天线 1、2、3、4 依次发送低频唤醒。
3. 钥匙收到 LF 后通过 RF 返回 5 字节数据帧。
4. 主控校验 RF 帧 CRC，并区分主动遥控帧和被动 RSSI 帧。
5. 主动 RF 帧直接执行开锁/闭锁。
6. 被动 RSSI 帧会转换为距离，并在四天线数据齐全后进行区域判断。
7. 如果 LF 窗口结束时仍有天线无回包，缺失天线按超远距离参与兜底判断。
8. 最终根据区域结果执行开锁或闭锁，并通过 CAN 上报过程状态。

## 主要模块

- `User/main.c`：系统初始化、主循环、门把手触发和 LF 窗口调度。
- `User/LF.c`：ATA5293 LF 唤醒发送和天线选择。
- `User/RF.c`：RF 数据采样和帧解码状态机。
- `User/pkes_core.c`：主动/被动 RF 帧处理、RSSI 样本管理、区域决策入口和锁动作决策。
- `User/pkes_ranging.c`：RSSI 距离转换、inside/outside 模板匹配和区域判断。
- `User/pkes_can_app.c`：CAN 业务帧队列和状态上报。
- `User/pkes_lock_app.c`：开锁、闭锁和 MOS 输出控制。

## CAN 报文

工程使用标准 CAN 帧，主要业务 ID：

- `0x301`：系统状态和执行结果。
- `0x302`：钥匙 ID、RSSI 和命令信息。
- `0x303`：单天线距离和融合区域结果。

## 当前说明

当前版本保留了原有裸机超级循环架构。中断侧主要负责 RF 采样和定时标志置位，耗时的 SPI、CAN、RSSI 处理和锁控制逻辑在主循环中执行。
