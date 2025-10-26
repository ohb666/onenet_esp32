#include "my_wifi_app.h"

static const char *TAG = "my_wifi_app";

SemaphoreHandle_t wifi_ok = NULL; // 创建一个信号量

void wifi_connected_thread(void *pvParameters)
{
    wifi_ok = xSemaphoreCreateBinary(); // 创建一个二值信号量
    if (wifi_ok == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }
    app_wifi_start();
    while (wifi_connected_already() != WIFI_STATUS_CONNECTED_OK)
    {
        ESP_LOGI(TAG, "wifi is connecting...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "wifi is connected!");
    xSemaphoreGive(wifi_ok); // 释放信号量
    vTaskDelete(NULL);
}

void my_wifi_app_start(void)
{
    xTaskCreate(wifi_connected_thread, "wifi_connected_thread", 2048, NULL, 5, NULL);
}