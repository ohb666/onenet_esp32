/**
 * @file tm_user.h
 * @brief 物模型用户接口定义文件
 * @details 该文件由物联网平台根据用户配置的物模型自动生成，包含设备服务、属性、命令和事件的接口定义。
 *          用户需实现文件中声明的回调函数，以处理物模型数据的读写、命令执行和事件上报等操作。
 * @author 中移物联网有限公司
 * @date 2025-06-13
 * @version V1.0
 * @copyright Copyright © 2024 中移物联网有限公司. All rights reserved.
 */
 
#ifndef __TM_USER_H__
#define __TM_USER_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "aiot_tm_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/* External Definition ( Constant and Macro )                                */
/*****************************************************************************/

/*****************************************************************************/
/* External Structures, Enum and Typedefs                                    */
/*****************************************************************************/
/****************************** Structure type *******************************/
struct prop_$OneNET_OneOS_VERSION_t
{
    int8_t *OneNET_OneOS_VERSION;
};


/****************************** Auto Generated *******************************/

/*****************************************************************************/
/* External Variables and Functions                                          */
/*****************************************************************************/
/*************************** Property Func List ******************************/
extern struct tm_prop_tbl_t tm_prop_list[];
extern uint16_t tm_prop_list_size;
/****************************** Auto Generated *******************************/

/**************************** Service Func List ******************************/
extern struct tm_svc_tbl_t tm_svc_list[];
extern uint16_t tm_svc_list_size;
/****************************** Auto Generated *******************************/

/**************************** Property Func Read ****************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_rd_cb(void *data);
int32_t tm_prop_charger1_current_rd_cb(void *data);
int32_t tm_prop_charger1_power_rd_cb(void *data);
int32_t tm_prop_charger1_space_rd_cb(void *data);
int32_t tm_prop_charger1_voltage_rd_cb(void *data);
int32_t tm_prop_charger2_current_rd_cb(void *data);
int32_t tm_prop_charger2_power_rd_cb(void *data);
int32_t tm_prop_charger2_space_rd_cb(void *data);
int32_t tm_prop_charger2_voltage_rd_cb(void *data);
int32_t tm_prop_latitude_rd_cb(void *data);
int32_t tm_prop_longitude_rd_cb(void *data);
int32_t tm_prop_over_pow_rd_cb(void *data);
int32_t tm_prop_relay1_rd_cb(void *data);
int32_t tm_prop_relay2_rd_cb(void *data);
int32_t tm_prop_temperature_rd_cb(void *data);

/****************************** Auto Generated *******************************/

/**************************** Service Func Invoke ****************************/

/****************************** Auto Generated *******************************/

/**************************** Property Func Write ****************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_wr_cb(void *data);
int32_t tm_prop_charger1_current_wr_cb(void *data);
int32_t tm_prop_charger1_power_wr_cb(void *data);
int32_t tm_prop_charger1_space_wr_cb(void *data);
int32_t tm_prop_charger1_voltage_wr_cb(void *data);
int32_t tm_prop_charger2_current_wr_cb(void *data);
int32_t tm_prop_charger2_power_wr_cb(void *data);
int32_t tm_prop_charger2_space_wr_cb(void *data);
int32_t tm_prop_charger2_voltage_wr_cb(void *data);
int32_t tm_prop_latitude_wr_cb(void *data);
int32_t tm_prop_longitude_wr_cb(void *data);
int32_t tm_prop_over_pow_wr_cb(void *data);
int32_t tm_prop_relay1_wr_cb(void *data);
int32_t tm_prop_relay2_wr_cb(void *data);
int32_t tm_prop_temperature_wr_cb(void *data);

/****************************** Auto Generated *******************************/

/**************************** Property Func Notify ***************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_notify(void *data, struct prop_$OneNET_OneOS_VERSION_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger1_current_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger1_power_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger1_space_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger1_voltage_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger2_current_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger2_power_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger2_space_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_charger2_voltage_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_latitude_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_longitude_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_over_pow_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_relay1_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_relay2_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms);
int32_t tm_prop_temperature_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms);

/****************************** Auto Generated *******************************/

/***************************** Event Func Notify *****************************/

/****************************** Auto Generated *******************************/

#ifdef __cplusplus
}
#endif

#endif
