#include "my_onenet.h"
#include <inttypes.h>

#define PRODUCT_ID     "******"     // OneNET产品ID
#define DEVICE_NAME    "******"    // 设备名称
#define ACCESS_KEY     "******"     // 设备密钥
#define CONNECT_TIMEOUT         30000     // 连接超时时间 (增加到30秒)
#define SEND_TIMEOUT    1000   // 数据发送时间 

#define TM_EXPIRE_TIME 1924833600              // Token过期时间(默认2030.12)

onenet_state_t onenet_stat= ONENET_DISCONNECTED;    // OneNET连接状态

// 添加互斥锁保护MQTT操作
static SemaphoreHandle_t mqtt_mutex = NULL;

static const char *TAG = "my_onenet";

void my_onenet_init()
{
    // 创建互斥锁
    if (mqtt_mutex == NULL) {
        mqtt_mutex = xSemaphoreCreateMutex();
        if (mqtt_mutex == NULL) {
            ESP_LOGE(TAG, "Failed to create MQTT mutex");
            return;
        }
    }

    ESP_LOGI(TAG, "Connecting to OneNET...");
    int ret = tm_login(PRODUCT_ID, DEVICE_NAME, ACCESS_KEY,
                   TM_EXPIRE_TIME, CONNECT_TIMEOUT); // 30秒超时
    if (ret != ERR_OK)
    {
        ESP_LOGE(TAG, "OneNET login failed: %d", ret);
        onenet_stat = ONENET_DISCONNECTED;
        return;
    }else
    {
    ESP_LOGI(TAG, "OneNET login success");
    onenet_stat = ONENET_CONNECTED;
    }

}

void onenet_log_thread(void*param)  // OneNET连接监控和重连线程
{
    xSemaphoreTake(wifi_ok, portMAX_DELAY); // 等待wifi连接成功
    ESP_LOGI(TAG, "Waiting for wifi connected...");

    int reconnect_attempts = 0;
    TickType_t last_connected_time = xTaskGetTickCount();

    while (1)
    {
        if (onenet_stat == ONENET_DISCONNECTED) // 如果OneNET连接断开，则重新连接
        {
            reconnect_attempts++;
            ESP_LOGI(TAG, "Attempting OneNET reconnection #%d...", reconnect_attempts);

            my_onenet_init();

            // 重连成功重置计数器
            if (onenet_stat == ONENET_CONNECTED) {
                ESP_LOGI(TAG, "OneNET reconnection successful after %d attempts", reconnect_attempts);
                reconnect_attempts = 0;
                last_connected_time = xTaskGetTickCount();
            } else {
                // 重连失败，等待更长时间
                vTaskDelay(CONNECT_TIMEOUT / portTICK_PERIOD_MS);
            }
        } else {
            // OneNET连接正常，正常监测
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void onenet_send_data(data_t* data)
{
    ESP_LOGW(TAG, "onenet_send_data is deprecated - send thread handles all operations");
    return;

    // 检查连接状态
    if (onenet_stat != ONENET_CONNECTED) {
        ESP_LOGE(TAG, "OneNET not connected, cannot send data");
        return;
    }

    // 获取互斥锁，最多等待5秒
    if (xSemaphoreTake(mqtt_mutex, pdMS_TO_TICKS(5000)) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to acquire MQTT mutex for sending data");
        return;
    }
    // 创建物模型数据实例
    void *property_data = tm_data_create();
    if (property_data == NULL) {
        ESP_LOGE(TAG, "Failed to create data instance");
        xSemaphoreGive(mqtt_mutex);
        return;
    }
    // 添加数据点
    switch (data->data_type)
    {
    case DATA_IS_FLOAT:
        tm_data_set_float(property_data, data->data_name, data->value.data_float, 0);
        break;

    case DATA_IS_INT:
        tm_data_set_int32(property_data, data->data_name, data->value.data_int, 0);
        break;

    case DATA_IS_BOOL:
        tm_data_set_bool(property_data, data->data_name, data->value.data_bool, 0);
        break;
    }

    // 发送数据
    int ret = tm_post_property(property_data, SEND_TIMEOUT);
    if (ret != ERR_OK) {
        ESP_LOGE(TAG, "Failed to send data: %d", ret);
        // 发送失败时，可能连接已断开，更新状态
        if (ret == -1000 || ret == -1) {
            onenet_stat = ONENET_DISCONNECTED;
            ESP_LOGW(TAG, "Connection may be lost, updating status");
        }
    } else {
        ESP_LOGI(TAG, "Data sent successfully");
    }

    // 释放互斥锁
    xSemaphoreGive(mqtt_mutex);
}

// 发送数据线程（统一处理所有MQTT通信）
void onenet_send_thread(void*param)
{
    // 等待OneNET完全就绪 - 增加到10秒
    ESP_LOGI(TAG, "Send thread waiting for OneNET to be fully ready...");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // 启动后等待5秒

    while(1)
    {
        if (onenet_stat == ONENET_CONNECTED)
        {
            ESP_LOGI(TAG, "Send thread: OneNET connected, attempting operations");

            data_t data;
            data.data_name = STR_CONST("temperature");
            data.data_type = DATA_IS_FLOAT;
            data.value.data_float = 25.5f;

            // 获取互斥锁进行发送操作
            if (xSemaphoreTake(mqtt_mutex, pdMS_TO_TICKS(5000)) == pdTRUE)
            {
                ESP_LOGI(TAG, "Send thread: MQTT mutex acquired for data transmission");

                // 发送数据
                void *property_data = tm_data_create();
                if (property_data != NULL) {
                    ESP_LOGI(TAG, "Send thread: Property data created successfully");

                    // 设置数据前验证指针有效性
                    int set_ret = tm_data_set_float(property_data, data.data_name, data.value.data_float, 0);
                    if (set_ret != ERR_OK) {
                        ESP_LOGE(TAG, "Send thread: Failed to set property data: %d", set_ret);
                        tm_data_delete(property_data);
                        property_data = NULL;
                        xSemaphoreGive(mqtt_mutex);
                        continue;
                    }

                    int send_ret = tm_post_property(property_data, SEND_TIMEOUT);

                    ESP_LOGI(TAG, "Send thread: tm_post_property returned: %d", send_ret);

                    if (send_ret != ERR_OK) {
                        ESP_LOGE(TAG, "Failed to send data: %d", send_ret);
                        switch (send_ret) {
                            case -1:
                                ESP_LOGE(TAG, "Error: Network connection lost");
                                break;
                            case -1000:
                                ESP_LOGE(TAG, "Error: Connection timeout");
                                break;
                            default:
                                ESP_LOGE(TAG, "Error: Unknown error code");
                                break;
                        }

                        if (send_ret == -1000 || send_ret == -1) {
                            onenet_stat = ONENET_DISCONNECTED;
                            ESP_LOGW(TAG, "Connection lost during send, status updated");
                        }

                        // 内存管理改进：让OneNET SDK自己管理内存
                        // 根据SDK文档，tm_post_property可能会在内部释放内存
                        // 因此，这里不再手动释放property_data
                    }
                }

                // 释放互斥锁
                xSemaphoreGive(mqtt_mutex);
                ESP_LOGI(TAG, "Send thread: MQTT mutex released");
            } else {
                ESP_LOGW(TAG, "Failed to acquire MQTT mutex for data transmission");
            }

            // 短暂延迟后继续下一次循环
            vTaskDelay(SEND_TIMEOUT / portTICK_PERIOD_MS); // 500ms间隔

        } else {   // 如果OneNET连接断开
            ESP_LOGI(TAG, "Send thread: OneNET disconnected, waiting...");
            vTaskDelay(2000 / portTICK_PERIOD_MS); // 等待2秒
        }
    }
}


// 注意：实际的数据接收回调函数在 tm_user.c 中实现
// SDK会自动调用 tm_prop_relay1_wr_cb() 等函数
// 您需要修改 components/onenet_esp32/src/onenet/tm/tm_user.c 文件

void onenet_rec_data()
{
    // 这个函数由OneNET SDK自动调用
    // 实际的数据接收通过tm_prop_*_wr_cb回调函数处理
    ESP_LOGI(TAG, "Received data from OneNET platform");
}

// OneNET连接状态监控线程
void onenet_receive_thread(void* param)
{
    while (1) {
        if (onenet_stat == ONENET_CONNECTED) {
            // 仅监控连接状态，不处理MQTT消息
            // 所有MQTT操作都由发送线程统一处理
            onenet_rec_data();
            vTaskDelay(100 / portTICK_PERIOD_MS); // 100ms检查间隔

        } else {
            // 未连接状态，等待更长时间
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void my_onenet_start()  //线程启动
{
    xTaskCreate(onenet_receive_thread, "onenet_recv", 4096, NULL, 7, NULL);
    xTaskCreate(onenet_send_thread, "onenet_send", 4096, NULL, 7, NULL);
    xTaskCreate(onenet_log_thread, "onenet_log", 4096, NULL, 6,NULL);
}
