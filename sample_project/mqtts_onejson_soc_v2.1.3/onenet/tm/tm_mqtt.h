/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file tm_mqtt.h
 * @brief MQTT protocol process with Thing Model data
 */

#ifndef __TM_MQTT_H__
#define __TM_MQTT_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< 数据接收回调函数指针 */
typedef int (*tm_mqtt_message_cb)(const uint8_t *data_name, uint8_t *data,
                               uint32_t data_len);

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
/**
 * @brief 初始化 MQTT 与物模型数据处理模块
 *
 * 该函数为 MQTT
 * 与物模型数据处理模块分配内存并进行初始化设置，同时注册数据接收回调函数。
 *
 * @param msg_cb 数据接收回调函数指针，当接收到 MQTT 消息时调用此函数。
 *               
 */
void tm_mqtt_init(tm_mqtt_message_cb msg_cb);

/**
 * @brief 反初始化 MQTT 与物模型数据处理模块
 *
 * 该函数释放初始化时分配的内存，清理资源。
 *
 */
void tm_mqtt_deinit(void);

/**
 * @brief 登录 MQTT 服务器
 *
 * 该函数使用提供的产品 ID、设备名称和设备令牌连接到 MQTT
 * 服务器，并订阅物模型相关主题。
 *
 * @param product_id 产品 ID，用于标识设备所属的产品。
 * @param dev_name 设备名称，用于唯一标识设备。
 * @param dev_token 设备令牌，用于身份验证。
 * @param timeout_ms 操作超时时间，单位为毫秒。
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_mqtt_login(const uint8_t *product_id, const uint8_t *dev_name,
                      const uint8_t *dev_token, uint32_t timeout_ms);

/**
 * @brief 登出 MQTT 服务器
 *
 * 该函数断开与 MQTT 服务器的连接，并释放订阅主题时分配的内存。
 *
 * @param timeout_ms 操作超时时间，单位为毫秒。
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_mqtt_logout(uint32_t timeout_ms);

/**
 * @brief 执行 MQTT 循环步骤
 *
 * 该函数用于处理 MQTT 消息的接收和发送，保持与 MQTT 服务器的通信。
 *
 * @param timeout_ms 操作超时时间，单位为毫秒。
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_mqtt_step(uint32_t timeout_ms);

/**
 * @brief 发送 MQTT 数据包
 *
 * 该函数将指定主题和负载的数据发送到 MQTT 服务器。
 *
 * @param topic 消息主题，指定消息的目标主题。
 * @param payload 消息负载，要发送的数据内容。
 * @param payload_len 消息负载的长度，单位为字节。
 * @param timeout_ms 操作超时时间，单位为毫秒。
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_mqtt_send_packet(const uint8_t *topic, uint8_t *payload,
                            uint32_t payload_len, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif