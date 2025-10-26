#ifndef MY_WIFI_APP_H
#define MY_WIFI_APP_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "my_wifi.h"


extern SemaphoreHandle_t wifi_ok; // 创建一个信号量
void wifi_connected_thread(void *pvParameters);
void my_wifi_app_start(void); // 启动wifi连接线程

#endif // MY_WIFI_APP_H
