#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "my_wifi_app.h"
#include "my_socket.h"
#include "my_onenet.h"



// static const char *TAG = "main"; // Unused variable, commented out

// void app_main(void)
// {
//     // 启动 WiFi
//     my_wifi_app_start();
//     my_onenet_start();

//     // 主循环
//     while (1) {
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }

void app_main(void) {
    // 启动 WiFi
    my_wifi_app_start();
    my_onenet_start();

    // 主循环
    while (1) {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}