/**
 * 版权所有 (c), 2012~2019 iot.10086.cn 保留所有权利
 *
 * @file mqtt_client.h
 * @brief 基于PahoMQTT MQTTClient-C\src\MQTTClient.h实现
 */

#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "MQTTPacket.h"
#include "mqtt_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* External Definition（Constant and Macro )                                 */
/*****************************************************************************/
#define MAX_MESSAGE_HANDLERS 5

#define DefaultClient                                                                                                  \
    {                                                                                                                  \
        0, 0, 0, 0, NULL, NULL, 0, 0, 0                                                                                \
    }

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
enum mqtt_ret_code
{
    BUFFER_OVERFLOW = -2,
    FAILURE         = -1,
    SUCCESS         = 0
};
#if 0
typedef struct mqtt_message_data
{
    struct mqtt_message_t* message;
    int8_t* topicName;
} mqtt_message_data;
#endif
typedef struct mqtt_conn_ack_data
{
    uint8_t rc;
    uint8_t sessionPresent;
} mqtt_conn_ack_data;

typedef struct mqtt_sub_ack_data
{
    enum mqtt_qos_e grantedQoS;
} mqtt_sub_ack_data;

typedef void (*message_handler)(void *, const uint8_t *, struct mqtt_message_t *);

typedef int32_t (*net_write_callback)(handle_t, void *, uint32_t, uint32_t);
typedef int32_t (*net_read_callback)(handle_t, void *, uint32_t, uint32_t);
typedef int32_t (*net_disconnect_callback)(handle_t);

typedef struct mqtt_network
{
    handle_t                handle;
    net_read_callback       mqttread;
    net_write_callback      mqttwrite;
    net_disconnect_callback disconnect;
} mqtt_network;

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
#if 1
/**
 * @brief 初始化MQTT客户端对象
 * @param network 网络配置结构体指针，包含网络读写和断开回调
 * @param sendbuf 发送缓冲区指针
 * @param sendbuf_size 发送缓冲区大小
 * @param readbuf 接收缓冲区指针
 * @param readbuf_size 接收缓冲区大小
 * @return 成功返回客户端对象指针，失败返回NULL
 * @note 调用此函数前需确保网络已连接
 */
void *mqtt_client_init(mqtt_network *network, uint8_t *sendbuf, size_t sendbuf_size, uint8_t *readbuf,
                       size_t readbuf_size);

/**
 * @brief 释放MQTT客户端资源
 * @param client 客户端对象指针
 * @note 调用此函数后client指针将不可用
 */
void mqtt_client_deinit(void *client);

/** 
 * MQTT连接 - 发送MQTT连接包并等待Connack响应
 * 调用前网络对象必须已连接到网络端点
 * @param options 连接选项
 * @return 成功代码
 */
/**
 * @brief 带返回结果的MQTT连接
 * @param client 客户端对象指针
 * @param options 连接参数结构体指针
 * @param data 连接确认返回结果结构体指针
 * @param timeout_ms 超时时间(毫秒)
 * @return 成功返回SUCCESS(0)，失败返回错误码
 * @note 调用前需确保网络已连接
 */
int32_t mqtt_client_connect_with_results(void *client, MQTTPacket_connectData *options, mqtt_conn_ack_data *data,
                                         uint32_t timeout_ms);

/** 
 * MQTT连接 - 发送MQTT连接包并等待Connack响应
 * 调用前网络对象必须已连接到网络端点
 * @param options 连接选项
 * @return 成功代码
 */
/**
 * @brief MQTT连接
 * @param client 客户端对象指针
 * @param options 连接参数结构体指针
 * @param timeout_ms 超时时间(毫秒)
 * @return 成功返回SUCCESS(0)，失败返回错误码
 * @note 调用前需确保网络已连接
 */
int32_t mqtt_client_connect(void *client, MQTTPacket_connectData *options, uint32_t timeout_ms);

/**
 * @brief MQTT消息发布
 * @param client 客户端对象指针
 * @param topicName 目标主题字符串
 * @param message 消息结构体指针
 * @param timeout_ms 超时时间(毫秒)
 * @return 成功返回SUCCESS(0)，失败返回错误码
 * @note QoS级别由message->qos指定
 */
int32_t mqtt_client_publish(void *client, const char *topicName, struct mqtt_message_t *message, uint32_t timeout_ms);

/**
 * @brief 设置或移除主题消息处理器
 * @param client 客户端对象指针
 * @param topic_filter 主题过滤器字符串
 * @param message_handler 消息处理回调函数指针，NULL表示移除
 * @param arg 传递给回调函数的参数
 * @return 成功返回SUCCESS(0)，失败返回错误码
 * @note 每个主题最多支持5个消息处理器
 */
int32_t mqtt_client_set_message_handler(void *client, const char *topic_filter, message_handler message_handler,
                                        void *arg);

/**
 * @brief 订阅主题并设置消息处理器
 * @param client 客户端对象指针
 * @param topic_filter 主题过滤器字符串
 * @param qos 订阅服务质量等级
 * @param message_handler 消息处理回调函数指针
 * @param arg 传递给回调函数的参数
 * @param timeout_ms 超时时间(毫秒)
 * @return 成功返回SUCCESS(0)，失败返回错误码
 */
int32_t mqtt_client_subscribe(void *c, const char *topic_filter, enum mqtt_qos_e qos, message_handler message_handler,
                              void *arg, uint32_t timeout_ms);

/**
 * MQTT订阅 - 发送MQTT订阅包并等待Suback响应
 * @param client 使用的客户端对象
 * @param topic_filter 要订阅的主题过滤器
 * @param message 要发送的消息
 * @param data 返回的Suback授予的QoS
 * @return 成功代码
 */
int32_t mqtt_client_subscribe_with_results(void *client, const char *topic_filter, enum mqtt_qos_e qos,
                                           message_handler message_handler, void *arg, mqtt_sub_ack_data *data,
                                           uint32_t timeout_ms);

/**
 * MQTT取消订阅 - 发送MQTT取消订阅包并等待Unsuback响应
 * @param client 使用的客户端对象
 * @param topic_filter 要取消订阅的主题过滤器
 * @return 成功代码
 */
int32_t mqtt_client_unsubscribe(void *client, const char *topic_filter, uint32_t timeout_ms);

/** MQTT Disconnect - send an MQTT disconnect packet and close the connection
 *  @param client - the client object to use
 *  @return success code
 */
int32_t mqtt_client_disconnect(void *client, uint32_t timeout_ms);

/** MQTT Yield - MQTT background
 *  @param client - the client object to use
 *  @param time - the time, in milliseconds, to yield for
 *  @return success code
 */
int32_t mqtt_client_yield(void *client, int32_t timeout_ms);

/** MQTT isConnected
 *  @param client - the client object to use
 *  @return truth value indicating whether the client is connected to the server
 */
int32_t mqtt_client_is_connected(void *client);
#endif

#ifdef __cplusplus
}
#endif

#endif
