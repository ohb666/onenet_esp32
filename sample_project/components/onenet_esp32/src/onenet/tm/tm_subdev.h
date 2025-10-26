/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file tm_subdev.h
 * @brief
 * 此文件定义了用于MQTT协议的子设备处理相关的接口和结构体，主要涉及子设备的属性操作、服务调用、拓扑关系管理以及登录登出等功能。
 */

#ifndef __TM_SUBDEV_H__
#define __TM_SUBDEV_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
// 包含数据类型定义头文件
#include <sys/types.h>

#include "common.h"
#include "data_types.h"
#include "dev_cardmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

// 如果配置了TM网关功能
#ifdef CONFIG_TM_GATEWAY

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/
// 此处可定义常量和宏，当前文件暂未定义

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/**
 * @struct tm_subdev_cbs
 * @brief
 * 定义子设备回调函数结构体，包含属性获取、属性设置、服务调用和拓扑关系处理的回调函数指针。
 */
struct tm_subdev_cbs {
  /**
   * @brief 属性获取回调函数
   * @param product_id 子设备的产品ID
   * @param dev_name 子设备的名称
   * @param props_list 需要获取的属性列表
   * @param props_data 用于存储获取到的属性数据的指针的指针
   * @return 0表示成功，其他值表示失败
   */
  int32_t (*subdev_props_get)(const uint8_t* product_id,
                              const uint8_t* dev_name,
                              const uint8_t* props_list, uint8_t** props_data);

  /**
   * @brief 属性设置回调函数
   * @param product_id 子设备的产品ID
   * @param dev_name 子设备的名称
   * @param props_data 需要设置的属性数据
   * @return 0表示成功，其他值表示失败
   */
  int32_t (*subdev_props_set)(const uint8_t* product_id,
                              const uint8_t* dev_name, uint8_t* props_data);

  /**
   * @brief 服务调用回调函数
   * @param product_id 子设备的产品ID
   * @param dev_name 子设备的名称
   * @param svc_id 服务ID
   * @param in_data 输入数据
   * @param out_data 用于存储输出数据的指针的指针
   * @return 0表示成功，其他值表示失败
   */
  int32_t (*subdev_service_invoke)(const uint8_t* product_id,
                                   const uint8_t* dev_name,
                                   const uint8_t* svc_id, uint8_t* in_data,
                                   uint8_t** out_data);

  /**
   * @brief 拓扑关系回调函数
   * @param topo_data 拓扑关系数据
   * @return 0表示成功，其他值表示失败
   */
  int32_t (*subdev_topo)(uint8_t* topo_data);
};

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/

/**
 * @brief 初始化网关子设备的属性设置、属性获取、服务调用和拓扑关系的回调函数
 * @param callbacks 包含回调函数指针的结构体
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_init(struct tm_subdev_cbs callbacks);

/**
 * @brief 绑定网关和子设备的关系
 * @param product_id 子设备的产品ID
 * @param dev_name 子设备的名称
 * @param access_key 子设备的登录密钥
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_add(const uint8_t* product_id, const uint8_t* dev_name,
                      const uint8_t* access_key, uint32_t timeout_ms);

/**
 * @brief 删除网关和子设备的绑定关系
 * @param product_id 子设备的产品ID
 * @param dev_name 子设备的名称
 * @param access_key 子设备的登录密钥
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_delete(const uint8_t* product_id, const uint8_t* dev_name,
                         const uint8_t* access_key, uint32_t timeout_ms);

/**
 * @brief 获取拓扑关系
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_topo_get(uint32_t timeout_ms);

/**
 * @brief 子设备登录
 * @param product_id 子设备的产品ID
 * @param dev_name 子设备的名称
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */

int32_t tm_subdev_login(const uint8_t* product_id, const uint8_t* dev_name,
                        uint32_t timeout_ms);

int32_t tm_subdev_login_with_cardmgr(const uint8_t* product_id,
                                     const uint8_t* dev_name,
                                     cardmgr_ctx_t* card_ctx,
                                     uint32_t timeout_ms);

/**
 * @brief 子设备登出
 * @param product_id 子设备的产品ID
 * @param dev_name 子设备的名称
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_logout(const uint8_t* product_id, const uint8_t* dev_name,
                         uint32_t timeout_ms);

/**
 * @brief 子设备数据上报
 * @param product_id 子设备的产品ID
 * @param dev_name 子设备的名称
 * @param prop_json 属性上报数据（JSON格式）
 * @param event_json 事件上报数据（JSON格式）
 * @param timeout_ms 操作超时时间（毫秒）
 * @return 0表示成功，其他值表示失败
 */
int32_t tm_subdev_post_data(const uint8_t* product_id, const uint8_t* dev_name,
                            uint8_t* prop_json, uint8_t* event_json,
                            uint32_t timeout_ms);

#endif

#ifdef __cplusplus
}
#endif

#endif