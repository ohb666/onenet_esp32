# MQTT 物模型 SDK 使用手册


## 概述
### 文档目的
本文档系统阐述如何基于SDK，运用MQTT协议的物模型功能，实现设备与OneNET平台之间的安全通信和数据交互，适用于设备端开发人员与集成工程师。

### 平台限制说明
如需了解平台MQTT协议限制的详细信息，请参见 [使用限制](https://open.iot.10086.cn/doc/v5/fuse/detail/921)。

### 数据流模型
如需了解平台MQTT协议数据流模型的详细信息，请参见 [MQTT 协议接入](https://open.iot.10086.cn/doc/v5/fuse/detail/919)。

### 术语定义
| 术语         | 定义说明                                                                 |
|--------------|--------------------------------------------------------------------------|
| 产品 ID      | OneNET 平台分配的产品唯一标识（`product_id`）                              |
| 设备名称     | 设备在平台注册的唯一标识符（`dev_name`）                                  |
| 登录令牌     | 设备身份认证凭证（`token`），详情可参考 [Token 算法](https://open.iot.10086.cn/doc/v5/fuse/detail/1486) |
| 主题名称     | 数据上报或订阅的目标路径（`topic`），更多信息请参见 [通信主题](https://open.iot.10086.cn/doc/v5/fuse/detail/920) |
| 有效载荷     | 实际传输的数据内容（`payload`），需严格遵循 OneJSON 格式规范，具体可查看 [OneJSON 格式规范](https://open.iot.10086.cn/doc/v5/fuse/detail/901) |
| 网关子设备   | 通过网关设备连接到 OneNET 平台的子设备，可实现属性设置、获取，服务调用以及拓扑关系管理等功能 |

## 开发准备  
### 工程目录结构
```shell
mqtts_onejson_soc/
├── 3rd                  # 第三方依赖库目录，包含SDK运行所需的基础组件
│   ├── cJSON            # JSON解析库，用于处理OneJSON格式数据
│   ├── paho-mqtt        # MQTT协议栈实现，提供MQTT通信核心功能
│   └── wolfssl          # TLS加密库，支持MQTTS安全连接
├── CMakeLists.txt       # CMake工程构建脚本，定义编译选项及依赖关系
├── common               # 公共工具模块，包含日志、链表、缓冲区等通用功能
├── examples             # 示例代码目录，演示SDK核心功能的使用方法
│   └── things_model     # 物模型功能示例，包含属性上报、事件触发等场景
├── onenet               # SDK核心功能目录，包含平台适配与协议实现
│   ├── platforms        # 平台适配层，实现不同硬件的TCP/时间等底层接口
│   ├── protocols        # 协议实现模块，封装MQTT协议交互逻辑
│   ├── security         # 安全模块，处理TLS证书验证与加密通信
│   ├── tm               # 物模型核心模块，实现属性/事件上报等功能
│   └── utils            # 工具函数库，包含Token生成、数据校验等辅助功能
└── readme.md            # 项目说明文档，包含环境要求、编译步骤等指南
```
### 环境要求
- **加密协议**：支持 TLS 1.2 及以上，用于 MQTTS 连接。
- **网络层**：支持 TCP socket 通信，实现短连接收发逻辑。

### 编译说明
#### 编译工具链
- 推荐使用 CMake 3.10 及以上版本进行工程构建。
- 支持 GCC、Clang 等主流 C 编译器，需开启 C99 及以上标准支持。  


### 编译说明  
#### 编译工具链  
- **推荐工具**：CMake 3.10+（工程构建），GCC/Clang 编译器（需开启 C99 标准）。  

#### 关键预处理宏
| 宏定义                    | 说明                                                                 |
|--------------------------|----------------------------------------------------------------------|
| `CONFIG_NETWORK_TLS`       | 启用TLS加密（MQTT over TLS；若未定义或者置0，则采用明文通信）    |
| `IOT_MQTT_SERVER_ADDR`     | MQTT服务器地址，默认为`"mqtts.heclouds.com"`                       |
| `IOT_MQTT_SERVER_PORT`     | MQTT服务器端口（TLS连接），默认为1883                               |
| `IOT_MQTT_SERVER_ADDR_TLS` | MQTT服务器地址，默认为`"mqttstls.heclouds.com"`                    |
| `IOT_MQTT_SERVER_PORT_TLS` | MQTT服务器端口（TLS连接），默认为8883                               |
| `SDK_PAYLOAD_LEN`          | SDK最大有效载荷长度，默认为4096字节                                 |
| `SDK_SEND_BUF_LEN`         | SDK发送缓冲区长度，默认为4096字节                                   |
| `SDK_RECV_BUF_LEN`         | SDK接收缓冲区长度，默认为4096字节                                   |
| `SDK_REQUEST_TIMEOUT`      | SDK请求超时时间，默认为4096毫秒                                     |
| `SDK_ACCESS_LIFE_TIME`     | SDK访问令牌有效期，默认为120秒                                      |
| `LOG_LEVEL`                | 日志级别，默认为`LOG_LEVEL_DEBUG`                                    |

#### 依赖库
- **加密库**：需集成 WolfSSL 3.15.3 及以上版本，用于 TLS 加密。
- **JSON 库**：cJSON（路径：`3rd/cJSON`），用于 OneJSON 数据解析。
- **MQTT协议库**：Paho MQTT（路径：`3rd/paho-mqtt`），提供MQTT协议实现。 

#### 编译命令示例  
```bash  
# 请确认 main.c 中产品 ID、设备名称及登录令牌已正确设置  
# 构建工程  
mkdir build && cd build  
cmake ..  
make  
```  

## 智能域名接入

### 功能概述
智能域名接入是OneNET平台提供的产品级动态域名解析服务，自2024年11月8日起对新创建产品默认启用。该服务通过将域名与产品ID强绑定，提升设备接入的灵活性和可维护性，推荐新设备优先采用。

> **注意**：传统固定域名（如`mqtts.heclouds.com`）仍可继续使用，但不具备智能域名的动态适配能力。

### 查询方法
1. 登录OneNET【开发者中心】
2. 进入【产品开发】栏目，选择目标产品
3. 在产品详情页的【基本信息】区域获取智能接入域名



### 域名格式说明
| 协议类型 | 域名格式模板 | 端口 | 加密方式 |
|----------|--------------|------|----------|
| MQTT | `{product_id}.mqtts.acc.cmcconenet.cn` | 1883 | 明文 |
| MQTT | `{product_id}.mqttstls.acc.cmcconenet.cn` | 8883 | TLS 1.2+ |
| NBIOT | `{product_id}.nbiotacc.acc.cmcconenet.cn` | 5684 | — |

> **参数说明**：`{product_id}`需替换为实际产品ID，长度为8-32位字符串
> **绑定关系**：每个产品拥有独立域名，切换产品时必须同步更新域名

### 配置示例
在`CMakeLists.txt`中配置智能域名：
```cmake
# MQTT over TLS 
add_definitions(-DIOT_MQTT_SERVER_ADDR_TLS="{product_id}.mqttstls.acc.cmcconenet.cn")
add_definitions(-DIOT_MQTT_SERVER_PORT_TLS=8883)

# MQTT 明文 
add_definitions(-DIOT_MQTT_SERVER_ADDR="{product_id}.mqtts.acc.cmcconenet.cn")
add_definitions(-DIOT_MQTT_SERVER_PORT=1883)
```

## 机卡管理服务配置
### 服务概述
如需了解平台机卡管理的详细信息，请参见[机卡管理](https://open.iot.10086.cn/doc/v5/fuse/detail/1492)

### 核心参数说明
| 术语                     | 定义说明                                 | 格式要求                                               |
| ---------------------- | ------------------------------------ | -------------------------------------------------- |
| 网络模式 | 网络类型标识                               | `0`：非蜂窝设备（WiFi/以太网等）<br>`1`：移动网络<br>`2`：电信网络<br>`3`：联通网络<br>`4`：广电网络 |
| 设备序列号(SN)          | 设备唯一标识（非蜂窝设备选填）                      | 字符串（示例：FACTORY-DEV-001）                            |
| 设备MAC地址(MAC)       | 设备物理地址（非蜂窝设备选填）                      | 6组冒号分隔的十六进制数（示例：00:11:22:33:44:55）                 |
| 国际移动设备识别码(IMEI)    | 蜂窝设备唯一标识（蜂窝设备必填）                     | 15位数字（示例：861234567890123）                          |
| 集成电路卡识别码(ICCID)    | 通信卡唯一标识（蜂窝设备选填，与IMSI/MSISDN至少三选其一）   | 19-20位数字（示例：89860012345678901234）                  |
| 国际移动用户识别码(IMSI)    | 移动用户唯一标识（蜂窝设备选填，与ICCID/MSISDN至少三选其一） | 15位数字（示例：460001234567890）                          |
| 物联卡号码(MSISDN)       | 开通的物联卡的卡号（蜂窝设备选填，与ICCID/IMSI至少三选其一）        | 11位数字（示例：13800138000）                              |

### 配置步骤
若需使用OneNET提供的机卡管理服务，可通过在`CMakeLists.txt`中添加预处理宏直接定义相关参数启用。

MQTTS协议SDK提供三种机卡协同服务启用方案：

**方案一：登录时上报**<br>
在MQTT登录鉴权阶段，通过clientid携带机卡协同信息上报

**方案二：登录后上报**<br>
设备登录成功后，向特定topic(`$sys/{product_id}/{dev_name}/cmp/property/post`)上报机卡协同信息

**方案三：子设备登录时上报**<br>
网关添加子设备时，在子设备登录流程中携带机卡协同信息上报

可通过在`CMakeLists.txt`中添加以下宏定义选择启用方案：
```cmake
// 机卡协同上报模式选择 (0:禁用 1:登录时上报 2:登录后上报 3:子设备登录时上报)
add_definitions(-DCONFIG_CARDMGR_MODE=1)
```

#### CMake配置示例
```cmake
// 非蜂窝设备配置（DEV_NETWORK_TYPE=0，若SN与MAC信息均为空，设备端将不会上报设备信息）
add_definitions(
    ...
    -DDEV_NETWORK_TYPE=0          // 网络类型：非蜂窝设备（Wi-Fi/以太网）
    -DCONFIG_CARDMGR_MODE=1       // 机卡管理服务信息上报模式选择（0:禁用 1:登录时上报 2:登录后上报 3:子设备登录时上报）
)

// 蜂窝设备配置（DEV_NETWORK_TYPE=1，移动卡）
add_definitions(
    ...
    -DDEV_NETWORK_TYPE=1          // 网络类型：移动卡设备
    -DCONFIG_CARDMGR_MODE=1       // 机卡管理服务信息上报模式选择（0:禁用 1:登录时上报 2:登录后上报 3:子设备登录时上报）
)
```

#### 代码文件示例
##### 直连设备初始化

- 非蜂窝设备（不带卡）初始化
```c
// 适用于Wi-Fi等无需SIM卡的设备
// 参数说明：
//   sn  - 设备序列号（可选，NULL表示不上报）
//   mac - 设备MAC地址（可选，NULL表示不上报）
const char* sn = "FACTORY-DEV-001";  // 工厂设备序列号示例
const char* mac = "00:11:22:33:44:55";  // MAC地址示例
cardmgr_ctx_init_for_nocard(sn, mac);
```
-  蜂窝设备（带卡）初始化
```c
// 适用于NB-IoT/4G等需要SIM卡的设备
// 参数要求：
//   imei     - 国际移动设备识别码（必填）
//   iccid/imsi/msisdn - 至少填写一项（SIM卡标识）
//   card_type - 卡类型（必填，使用DEV_NETWORK_TYPE宏）
const char* imei = "861234567890123";    // 设备唯一标识（必填）
const char* iccid = "89860012345678901234";  // SIM卡ICCID示例
const char* imsi = "460001234567890";     // SIM卡IMSI示例
const char* msisdn = NULL;               // 可选，无则设为NULL
card_type_e card_type = (card_type_e)DEV_NETWORK_TYPE;  // 卡类型
cardmgr_ctx_init_for_card(imei, 
                          iccid, 
                          imsi, 
                          msisdn,
                          card_type);
```

##### 子设备初始化
```c
    // 子设备
    struct tm_subdev_cbs subdev_cbs = {
        .subdev_props_get = usr_subdev_props_get,
        .subdev_props_set = usr_subdev_props_set,
        .subdev_service_invoke = usr_subdev_service_invoke,
        .subdev_topo = usr_subdev_topo
    };

    const char *subproduct_id = "PPjDKZpxI3";
    const char *subdev_name = "ID01";
    const char *subaccess_key = "b2tvR3JPZGpuRnBwTFNmSW5FOHMwYlhvNmJEanRsTko=";

    tm_subdev_init(subdev_cbs);
    {
        // 不带卡设备中，sn与mac为空，设备端将不会上报设备信息
        const char* sn = "FACTORY-DEV-001";
        const char* mac = "00:11:22:33:44:55";
        // 非蜂窝设备
        cardmgr_ctx_init_for_nocard(sn, mac);
    }

    // !!! 非蜂窝设备与蜂窝设备选择其一初始化
    {
        // 带卡设备中，imei为必填，iccid/imsi/msisdn至少填写一项
        const char* imei = "861234567890123";
        const char* iccid = "89860012345678901234";
        const char* imsi = "460001234567890";
        const char* msisdn = NULL;
        // 蜂窝设备
        cardmgr_ctx_init_for_card(imei,
                                iccid,
                                imsi,
                                msisdn,
                                (card_type_e)DEV_NETWORK_TYPE);
    }

    int ret = tm_subdev_add(subproduct_id, subdev_name, subaccess_key, 3000);
    if (ERR_OK != ret) {
        logw("ThingModel subdev add failed!");
    }

    ret = tm_subdev_login(subproduct_id, subdev_name, 3000);
    if (ERR_OK != ret) {
        CHECK_EXPR_GOTO(ERR_OK != ret, _END, "ThingModel subdev login failed!");
    }

    cardmgr_ctx_destroy();
```
### 注意事项
1. **参数校验规则**：蜂窝设备必须填写DEV_IMEI（15位数字），且DEV_ICCID/DEV_IMSI/DEV_MSISDN至少填写一项（分别满足19-20位/15位/11位数字要求）；非蜂窝设备可选填DEV_SN（字符串）和DEV_MAC（6组冒号分隔十六进制数）。
2. **平台适配**：确保`DEV_NETWORK_TYPE`与OneNET平台产品类型（蜂窝/非蜂窝）配置一致，否则可能导致认证失败。

## 接口参考
### tm_api.h（物模型核心接口）
#### 关键接口
| 接口名称                | 功能概述                     | 详细说明                     |
|-------------------------|------------------------------|------------------------------|
| `tm_init`         | 初始化设备并注册下行数据回调   | 详情参见 `tm_api.h` 函数注释。 |
| `tm_deinit`       | 反初始化设备                   | 详情参见 `tm_api.h` 函数注释。 |
| `tm_login`        | 设备登录 OneNET 平台           | 详情参见 `tm_api.h` 函数注释。 |
| `tm_logout`       | 设备登出 OneNET 平台           | 详情参见 `tm_api.h` 函数注释。 |
| `tm_post_event`   | 上报设备事件数据（OneJSON 格式）| 详情参见 `tm_api.h` 函数注释。 |
| `tm_get_desired_props` | 获取期望属性数据 | 详情参见 `tm_api.h` 函数注释。 |
| `tm_delete_desired_props` | 删除期望属性数据 | 详情参见 `tm_api.h` 函数注释。 |
| `tm_pack_device_data` | 封装设备的属性和事件数据，可用于子设备 | 详情参见 `tm_api.h` 函数注释。 |
| `tm_post_pack_data` | 上报封装后的数据 | 详情参见 `tm_api.h` 函数注释。 |
| `tm_post_history_data` | 上报历史数据 | 详情参见 `tm_api.h` 函数注释。 |
| `tm_step` | 执行设备的一步操作 | 详情参见 `tm_api.h` 函数注释。 |

### tm_mqtt.h（MQTT 协议接口）
#### 核心功能
封装 MQTT 协议栈操作，支持 TLS 加密连接、数据收发及重连逻辑。

#### 关键接口
| 接口名称          | 功能概述                     | 详细说明                     |
|-------------------|------------------------------|------------------------------|
| `tm_mqtt_init`    | 初始化 MQTT 协议栈及接收回调   | 详情参见 `tm_mqtt.h` 函数注释。 |
| `tm_mqtt_deinit`  | 反初始化 MQTT 协议栈           | 详情参见 `tm_mqtt.h` 函数注释。 |
| `tm_mqtt_login`   | 通过 MQTT 协议登录平台         | 详情参见 `tm_mqtt.h` 函数注释。 |
| `tm_mqtt_logout`  | 通过 MQTT 协议登出平台         | 详情参见 `tm_mqtt.h` 函数注释。 |
| `tm_mqtt_step`    | 处理 MQTT 协议事件（如重连）   | 详情参见 `tm_mqtt.h` 函数注释。 |
| `tm_mqtt_send_packet` | 向指定主题发送 MQTT 消息      | 详情参见 `tm_mqtt.h` 函数注释。 |

#### 加密连接配置
- 启用 TLS 时，需确保 `CONFIG_NETWORK_TLS = 1`，并使用默认 TLS 服务器地址 `mqttstls.heclouds.com` 和端口 8883。

### plat_tcp.h（TCP 底层接口，需平台适配）
#### 核心功能
提供 TCP 连接建立、数据收发、断开连接等基础接口，需根据硬件平台（如 STM32、ESP32）实现具体逻辑。

#### 必选接口
| 接口名称          | 功能概述                     | 详细说明                     |
|-------------------|------------------------------|------------------------------|
| `plat_tcp_connect`| 创建 TCP 连接句柄             | 详情参见 `plat_tcp.h` 函数注释，需适配平台 socket 连接逻辑。 |
| `plat_tcp_send`   | 通过连接发送数据               | 详情参见 `plat_tcp.h` 函数注释，需处理分包与超时重传。 |
| `plat_tcp_recv`   | 从连接接收数据                 | 详情参见 `plat_tcp.h` 函数注释，需实现缓冲区管理与超时机制。 |
| `plat_tcp_disconnect` | 断开指定的 TCP 连接并释放资源 | 详情参见 `plat_tcp.h` 函数注释，需适配平台 socket 关闭逻辑。 |

## 注意事项
### 预处理宏配置
- 务必正确定义 `IOT_MQTT_SERVER_ADDR_TLS` 和 `IOT_MQTT_SERVER_PORT_TLS`，确保指向 OneNET 平台 MQTT 服务地址。
- 启用 TLS 时，需确保 `CONFIG_NETWORK_TLS = 1`，确保 TLS 加密功能生效。

### 数据格式规范
- 所有上报数据必须严格遵循 OneJSON 格式规范，否则会导致解析失败或平台拒绝接收。

