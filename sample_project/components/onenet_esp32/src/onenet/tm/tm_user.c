 /**
 * @file tm_user.c
 * @brief 物模型用户接口定义文件
 * @details 该文件由物联网平台根据用户配置的物模型自动生成，包含设备服务、属性、命令和事件的接口定义。
 *          用户需实现文件中声明的回调函数，以处理物模型数据的读写、命令执行和事件上报等操作。
 * @author 中移物联网有限公司
 * @date 2025-06-13
 * @version V1.0
 * @copyright Copyright © 2024 中移物联网有限公司. All rights reserved.
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "tm_data.h"
#include "tm_user.h"
#include "esp_log.h"
#include "cJSON.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
/*************************** Property Func List ******************************/
struct tm_prop_tbl_t tm_prop_list[] = {
    TM_PROPERTY_RW($OneNET_OneOS_VERSION),
    TM_PROPERTY_RW(charger1_current),
    TM_PROPERTY_RW(charger1_power),
    TM_PROPERTY_RW(charger1_space),
    TM_PROPERTY_RW(charger1_voltage),
    TM_PROPERTY_RW(charger2_current),
    TM_PROPERTY_RW(charger2_power),
    TM_PROPERTY_RW(charger2_space),
    TM_PROPERTY_RW(charger2_voltage),
    TM_PROPERTY_RW(latitude),
    TM_PROPERTY_RW(longitude),
    TM_PROPERTY_RW(over_pow),
    TM_PROPERTY_RW(relay1),
    TM_PROPERTY_RW(relay2),
    TM_PROPERTY_RW(temperature)
};
uint16_t tm_prop_list_size = ARRAY_SIZE(tm_prop_list);
/****************************** Auto Generated *******************************/

/***************************** Service Func List *******************************/
struct tm_svc_tbl_t tm_svc_list[] = {0};
uint16_t tm_svc_list_size = 0;

/****************************** Auto Generated *******************************/

/**************************** Property Func Read *****************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger1_current_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger1_power_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger1_space_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger1_voltage_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger2_current_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger2_power_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger2_space_rd_cb(void *data) { return 0; }
int32_t tm_prop_charger2_voltage_rd_cb(void *data) { return 0; }
int32_t tm_prop_latitude_rd_cb(void *data) { return 0; }
int32_t tm_prop_longitude_rd_cb(void *data) { return 0; }
int32_t tm_prop_over_pow_rd_cb(void *data) { return 0; }
int32_t tm_prop_relay1_rd_cb(void *data) { return 0; }
int32_t tm_prop_relay2_rd_cb(void *data) { return 0; }
int32_t tm_prop_temperature_rd_cb(void *data) { return 0; }

/**************************** Property Func Write ****************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger1_current_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger1_power_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger1_space_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger1_voltage_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger2_current_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger2_power_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger2_space_wr_cb(void *data) { return 0; }
int32_t tm_prop_charger2_voltage_wr_cb(void *data) { return 0; }
int32_t tm_prop_latitude_wr_cb(void *data) { return 0; }
int32_t tm_prop_longitude_wr_cb(void *data) { return 0; }
int32_t tm_prop_over_pow_wr_cb(void *data) { return 0; }
int32_t tm_prop_relay1_wr_cb(void *data){ return 0; }
int32_t tm_prop_relay2_wr_cb(void *data){ return 0; }

int32_t tm_prop_temperature_wr_cb(void *data)
{
    // 增强调试信息 - 检查数据指针有效性
    if (data == NULL) {
        ESP_LOGE("TM", "rec:temperature: data pointer is NULL!");
        return -1;
    }

    // 正确的cJSON解析方式
    // OneNET SDK传递的是cJSON对象，需要解析其中的value字段
    float64_t temp_value = 0.0;

    // 检查是否是cJSON对象格式
    if (((cJSON *)data)->type == cJSON_Object) {
        cJSON *value_item = cJSON_GetObjectItem((cJSON *)data, "value");
        if (value_item != NULL) {
            temp_value = value_item->valuedouble;
            ESP_LOGI("TM", "rec:temperature: parsed from cJSON object value = %.6f", temp_value);
        } else {
            ESP_LOGE("TM", "rec:temperature: cJSON object has no 'value' field!");
            return -1;
        }
    } 
    // 打印cJSON对象信息
    ESP_LOGI("TM", "rec:temperature: cJSON type = %d, string = %s",
             ((cJSON *)data)->type,
             ((cJSON *)data)->string ? ((cJSON *)data)->string : "NULL");

    ESP_LOGI("TM", "rec:temperature: Final parsed value = %.6f", (float32_t)temp_value);
    return 0;
}

/**************************** Property Func Notify ***************************/
int32_t tm_prop_$OneNET_OneOS_VERSION_notify(void *data, struct prop_$OneNET_OneOS_VERSION_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger1_current_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger1_power_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger1_space_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger1_voltage_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger2_current_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger2_power_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger2_space_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_charger2_voltage_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_latitude_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_longitude_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_over_pow_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_relay1_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_relay2_notify(void *data, int32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
int32_t tm_prop_temperature_notify(void *data, float32_t val, uint64_t timestamp, uint32_t timeout_ms) { return 0; }
