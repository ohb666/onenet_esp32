# ESP32 OneNET IoT 项目

一个基于ESP32-S3和OneNET云平台的完整IoT数据采集与控制解决方案，实现了设备数据上报、云端指令接收、实时监控等功能。

## 🎯 项目特色

- **稳定可靠的OneNET连接**：解决了多线程MQTT冲突问题，确保长期稳定运行
- **高效数据上报**：支持自定义频率的数据上报（默认1000ms间隔）
- **双向通信**：支持从OneNET平台接收设备控制指令
- **完整的错误处理**：包含连接断开重连、数据发送失败处理等


## 🏗️ 技术架构

### 硬件平台

- **主控芯片**：ESP32-S3
- **开发环境**：ESP-IDF v5.1.2
- **连接方式**：Wi-Fi + MQTT
- **云平台**：中国移动OneNET

### 软件架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   WiFi 连接     │    │  OneNET 连接    │    │   数据发送      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
       │                      │                      │
       └──────────────────────┼──────────────────────┘
                              │
                    ┌─────────────────┐
                    │   线程管理     │
                    │   互斥锁保护    │
                    │   错误处理      │
                    └─────────────────┘
```

## 🚀 核心功能

### 1. 数据上报

- 支持浮点数、整数、布尔值等多种数据类型
- 自定义上报频率（通过`SEND_TIMEOUT`配置）
- 自动重连机制
- 数据发送状态监控

### 2. 指令接收

- 支持OneNET平台下发指令
- JSON数据解析处理
- 实时响应控制指令
- 详细的接收调试日志

### 3. 连接管理

- 自动WiFi连接
- OneNET自动登录
- 连接状态实时监控
- 断线自动重连

### 4. 线程安全

- 多线程架构设计
- 互斥锁保护MQTT操作
- 避免线程冲突问题

## 📁 项目结构

```
sample_project/
├── main/
│   └── myapp/
│       ├── my_onenet.c      # OneNET核心实现
│       ├── my_onenet.h      # 头文件定义
│       └── main.c           # 主程序入口
├── components/
│   └── onenet_esp32/        # OneNET SDK
├── docs/                    # 文档目录
│   ├── ONENET_API_DOCUMENTATION.md
│   ├── MEMORY_FIX_SUMMARY.md
│   └── ...
└── README.md               # 项目说明
```

## ⚙️ 配置参数

### OneNET 平台配置

```c
#define PRODUCT_ID     "*****"     // OneNET产品ID
#define DEVICE_NAME    "*****"    // 设备名称
#define ACCESS_KEY     "*****"     // 设备密钥
```

### Wifi配置
Wifi的连接直接通过menuconfig中去进行修改即可

### 发送配置

```c
#define SEND_TIMEOUT    500    // 数据发送间隔(ms)
#define CONNECT_TIMEOUT 30000   // 连接超时时间(ms)
```

## 🔧 编译和烧录

### 环境要求

- ESP-IDF v5.1.2 或更高版本
- 支持ESP32-S3的开发板

### 编译命令

```bash
idf.py build
idf.py flash monitor
```

## 📊 运行效果

### 正常运行日志

```
I (xxxx) my_wifi_app: wifi is connected!
I (xxxx) my_onenet: OneNET login success
I (xxxx) my_onenet: Send thread: MQTT mutex acquired for data transmission
I (xxxx) my_onenet: Data sent successfully to OneNET!
I (xxxx) TM: rec:temperature: Final parsed value = 25.400000
```

### 性能指标

- **启动时间**：< 10秒
- **数据上报频率**：500ms（可配置）
- **内存占用**：约250KB可用堆内存
- **连接稳定性**：24/7 稳定运行

## 🐛 解决的技术问题

1. **MQTT线程冲突**：通过统一MQTT操作到单线程解决
2. **时间计算错误**：修正FreeRTOS时间计算
3. **数据解析问题**：正确实现cJSON数据解析

## 📖 文档

具体可以查看库中的md手册介绍

## 🤝 贡献

欢迎提交Issue和Pull Request来改进这个项目！

## 📄 许可证

本项目仅供学习和参考使用。

---

**注意**：请确保将上述配置参数中的实际值替换为您自己的OneNET平台信息。
