/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file tm_api.h
 * @brief 多协议物模型 API 头文件，提供设备与平台交互的基础接口
 */

#ifndef __TM_API_H__
#define __TM_API_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#include "data_types.h"

#include "common.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/
// 定义 ARRAY_SIZE 宏，用于计算数组的元素个数
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

// 定义 ADD_MSG2PAYLOAD 宏，用于将消息添加到有效负载中
// 如果添加后的有效负载长度超过 SDK_PAYLOAD_LEN，则输出错误日志
#ifndef ADD_MSG2PAYLOAD
#define ADD_MSG2PAYLOAD(payload, _msg)                                         \
  do {                                                                         \
    int payload_len = osl_strlen(payload);                                     \
    int _msg_len = osl_strlen(_msg);                                           \
    if (SDK_PAYLOAD_LEN > payload_len + _msg_len) {                            \
      osl_strcat(payload, _msg);                                               \
    } else {                                                                   \
      loge("payload length(%d) more than the "                                 \
           "SDK_PAYLOAD_LEN(%d)",                                              \
           payload_len + _msg_len, SDK_PAYLOAD_LEN);                           \
    }                                                                          \
  } while (0)
#endif

// 定义 TM_PROPERTY_RW 宏，用于定义可读写的属性表项
#define TM_PROPERTY_RW(x)                                                      \
  { #x, tm_prop_##x##_rd_cb, tm_prop_##x##_wr_cb }

// 定义 TM_PROPERTY_RO 宏，用于定义只读的属性表项
#define TM_PROPERTY_RO(x)                                                      \
  { #x, tm_prop_##x##_rd_cb, NULL }

// 定义 TM_SERVICE 宏，用于定义服务表项
#define TM_SERVICE(x)                                                          \
  { #x, tm_svc_##x##_cb }

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/

/**
 * @brief 设备属性读取回调函数类型
 *
 * 该回调函数用于读取设备特定属性的值。
 *
 * @param res 指向存储属性值的结果缓冲区的指针。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_prop_read_cb)(void *res);

/**
 * @brief 设备属性写入回调函数类型
 *
 * 该回调函数用于向设备特定属性写入新值。
 *
 * @param res 指向要写入属性的新值的指针。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_prop_write_cb)(void *res);

/**
 * @brief 设备事件读取回调函数类型
 *
 * 该回调函数用于读取设备特定事件的详细信息。
 *
 * @param res 指向存储事件详细信息的结果缓冲区的指针。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_event_read_cb)(void *res);

/**
 * @brief 设备服务调用回调函数类型
 *
 * 该回调函数用于调用设备的特定服务。
 *
 * @param in 指向服务调用输入数据的指针。
 * @param out 指向存储服务调用结果的输出数据缓冲区的指针。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_svc_invoke_cb)(void *in, void *out);

/**
 * @brief 设备消息解析回调函数类型
 *
 * 该回调函数用于解析接收到的设备消息。
 *
 * @param data_name 被解析数据的名称。
 * @param data 指向数据缓冲区的指针。
 * @param data_len 数据缓冲区的长度。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_msg_parse_cb)(const uint8_t *data_name, uint8_t *data,
                                   uint32_t data_len);

/**
 * @brief 设备属性表结构
 *
 * 该结构包含设备属性的相关信息，包括属性名称和读写回调函数。
 */
struct tm_prop_tbl_t {
  const uint8_t *name;            /**< 属性名称 */
  tm_prop_read_cb tm_prop_rd_cb;  /**< 属性读取回调函数 */
  tm_prop_write_cb tm_prop_wr_cb; /**< 属性写入回调函数 */
};

/**
 * @brief 设备服务表结构
 *
 * 该结构包含设备服务的相关信息，包括服务名称和调用回调函数。
 */
struct tm_svc_tbl_t {
  const uint8_t *name;        /**< 服务名称 */
  tm_svc_invoke_cb tm_svc_cb; /**< 服务调用回调函数 */
};

/**
 * @brief 下行数据处理回调表结构
 *
 * 该结构包含属性表和服务表的指针，以及它们的大小，用于处理下行数据。
 */
struct tm_downlink_tbl_t {
  struct tm_prop_tbl_t *prop_tbl; /**< 指向设备属性表的指针 */
  struct tm_svc_tbl_t *svc_tbl;   /**< 指向设备服务表的指针 */
  uint16_t prop_tbl_size;         /**< 属性表的大小 */
  uint16_t svc_tbl_size;          /**< 服务表的大小 */
};

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/


/**
 * @brief 设备反初始化函数
 *
 * 该函数对设备进行反初始化，释放初始化期间分配的所有资源。
 *
 * @return 0表示成功，其他值表示失败
 * @note 反初始化后，设备需要重新初始化才能使用。
 */
int32_t tm_deinit(void);

/**
 * @brief 向平台发起设备登录请求
 *
 * 该函数使用提供的产品
 * ID、设备名称、访问密钥、过期时间和超时时间向平台发送登录请求，尝试对设备进行身份验证。
 *
 * @param product_id 产品 ID。
 * @param dev_name 设备名称。
 * @param access_key 产品密钥或设备密钥。
 * @param expire_time 登录令牌的过期时间。
 * @param timeout_ms 登录超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果登录失败，设备可能无法访问平台的服务。
 */
int32_t tm_login(const char *product_id, const char *dev_name,
                 const char *access_key, uint64_t expire_time,
                 uint32_t timeout_ms);


/**
 * @brief 设备登出函数
 *
 * 该函数使用提供的超时时间向平台发送登出请求，终止设备与平台的会话。
 *
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 登出后，设备需要重新登录才能访问平台的服务。
 */
int32_t tm_logout(uint32_t timeout_ms);

/**
 * @brief 向平台上报设备属性
 *
 * 该函数使用指定的超时时间将提供的设备属性数据发送到平台。
 *
 * @param prop_data 指向设备属性数据的指针。
 * @param timeout_ms 超时时间（毫秒）。
 *@return 0表示成功，其他值表示失败
 * @note 如果上报失败，平台可能无法收到更新后的设备属性。
 */
int32_t tm_post_property(void *prop_data, uint32_t timeout_ms);

/**
 * @brief 向平台上报设备事件
 *
 * 该函数使用指定的超时时间将提供的设备事件数据发送到平台。
 *
 * @param event_data 指向设备事件数据的指针。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果上报失败，平台可能无法收到设备事件。
 */
int32_t tm_post_event(void *event_data, uint32_t timeout_ms);

/**
 * @brief 从平台获取期望属性
 *
 * 该函数使用指定的超时时间向平台请求期望属性。
 *
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果请求失败，设备可能无法收到平台的期望属性。
 */
int32_t tm_get_desired_props(uint32_t timeout_ms);

/**
 * @brief 从平台删除期望属性
 *
 * 该函数使用指定的超时时间向平台发送删除期望属性的请求。
 *
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果删除失败，期望属性可能仍保留在平台上。
 */
int32_t tm_delete_desired_props(uint32_t timeout_ms);

/**
 * @brief 打包设备的属性和事件数据，适用于子设备
 *
 * 该函数用于打包设备的属性和事件数据，供子设备使用。如果目标指针为空，将在内部分配空间并通过返回值返回地址。
 *
 * @param data 要打包的目标指针地址，用于后续调用上报接口。当设置为 NULL
 * 时，接口将在内部分配空间，并通过返回值返回地址。
 * @param product_id 需要打包数据的产品 ID。
 * @param dev_name 需要打包数据的设备名称。
 * @param prop 传入的属性数据，支持 json 格式（as_raw 设置为 1），也可以像
 * tm_user.c 文件中那样使用 tm_data API 构造数据（as_raw 设置为 0）。
 * @param event 与 prop 定义相同，用于传入事件数据。
 * @param as_raw 是否为原始 json 格式数据。
 * @return 打包后的数据指针地址。
 * @note 如果打包失败，返回的指针可能为空。
 */
void *tm_pack_device_data(void *data, const uint8_t *product_id,
                          const uint8_t *dev_name, void *prop, void *event,
                          int8_t as_raw);

/**
 * @brief 向平台上报打包后的设备数据
 *
 * 该函数使用指定的超时时间将打包后的设备数据发送到平台。
 *
 * @param pack_data 指向打包后设备数据的指针。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果上报失败，平台可能无法收到打包后的设备数据。
 */
int32_t tm_post_pack_data(void *pack_data, uint32_t timeout_ms);

/**
 * @brief 向平台上报设备历史数据
 *
 * 该函数使用指定的超时时间将提供的设备历史数据发送到平台。
 *
 * @param history_data 指向设备历史数据的指针。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果上报失败，平台可能无法收到设备历史数据。
 */
int32_t tm_post_history_data(void *history_data, uint32_t timeout_ms);

#ifdef CONFIG_TM_GATEWAY
/**
 * @brief 子设备消息处理回调函数类型
 *
 * 该回调函数用于处理从子设备接收到的消息。
 *
 * @param name 子设备名称。
 * @param data 指向消息数据的指针。
 * @param data_len 消息数据的长度。
 * @return 0表示成功，其他值表示失败
 */
typedef int32_t (*tm_subdev_cb)(const uint8_t *name, void *data,
                                uint32_t data_len);

/**
 * @brief 设置子设备消息处理回调函数
 *
 * 该函数设置当接收到子设备消息时将调用的回调函数。
 *
 * @param callback 指向回调函数的指针。
 * @return 0表示成功，其他值表示失败
 * @note 如果未设置回调函数，子设备消息可能无法正确处理。
 */
int32_t tm_set_subdev_callback(tm_subdev_cb callback);

/**
 * @brief 向平台发送原始数据
 *
 * 该函数使用指定的超时时间将提供的原始数据发送到平台，同时，如果有可用的回复数据，将其存储在
 * reply_data 中，并将其长度存储在 reply_data_len 中。
 *
 * @param name 数据或服务的名称。
 * @param raw_data 指向原始数据的指针。
 * @param raw_data_len 原始数据的长度。
 * @param reply_data 指向存储回复数据的指针的指针。
 * @param reply_data_len 指向存储回复数据长度的变量的指针。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果发送失败，reply_data 和 reply_data_len 可能无效。
 */
int32_t tm_post_raw(const uint8_t *name, uint8_t *raw_data,
                    uint32_t raw_data_len, uint8_t **reply_data,
                    uint32_t *reply_data_len, uint32_t timeout_ms);

/**
 * @brief 向平台发送请求
 *
 * 该函数使用提供的数据和超时时间向平台发送请求，同时，如果有可用的回复数据，将其存储在
 * reply_data 中，并将其长度存储在 reply_data_len 中。
 *
 * @param name 请求或服务的名称。
 * @param as_raw 标志，指示数据是否为原始 json 格式。
 * @param data 指向请求数据的指针。
 * @param data_len 请求数据的长度。
 * @param reply_data 指向存储回复数据的指针的指针。
 * @param reply_data_len 指向存储回复数据长度的变量的指针。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果请求失败，reply_data 和 reply_data_len 可能无效。
 */
int32_t tm_send_request(const uint8_t *name, uint8_t as_raw, void *data,
                        uint32_t data_len, void **reply_data,
                        uint32_t *reply_data_len, uint32_t timeout_ms);

/**
 * @brief 向平台发送响应
 *
 * 该函数使用提供的消息 ID、代码、数据和超时时间向平台发送响应。
 *
 * @param name 响应或服务的名称。
 * @param msg_id 指向消息 ID 的指针。
 * @param msg_code 消息代码，指示响应的状态。
 * @param as_raw 标志，指示数据是否为原始 json 格式。
 * @param resp_data 指向响应数据的指针。
 * @param resp_data_len 响应数据的长度。
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果响应发送失败，平台可能无法收到正确的响应。
 */
int32_t tm_send_response(const uint8_t *name, uint8_t *msg_id, int32_t msg_code,
                         uint8_t as_raw, void *resp_data,
                         uint32_t resp_data_len, uint32_t timeout_ms);
#endif

/**
 * @brief 执行设备操作的单步处理
 *
 * 该函数执行设备操作的单步处理，例如处理传入消息或维护与平台的连接。
 *
 * @param timeout_ms 超时时间（毫秒）。
 * @return 0表示成功，其他值表示失败
 * @note 如果单步处理失败，设备的正常操作可能会受到影响。
 */
int32_t tm_step(uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif