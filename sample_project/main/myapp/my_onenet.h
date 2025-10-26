#ifndef MY_ONENET_H
#define MY_ONENET_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "my_wifi_app.h"
#include "onenet_esp32.h"
#include "aiot_tm_api.h"
#include "err_def.h"
#include "tm_data.h"

typedef enum {
    ONENET_DISCONNECTED = 0,
    ONENET_CONNECTED = 1,
    ONENET_RECONNECTING = 2
} onenet_state_t;

typedef enum {
    DATA_IS_FLOAT = 0,
    DATA_IS_INT = 1,
    DATA_IS_BOOL = 2
} data_type;

// 简化字符串转换的宏
#define STR_CONST(s) ((const int8_t *)(s))

typedef struct {
    const int8_t *data_name;
    data_type data_type;
    union {
        float data_float;
        int data_int;
        bool data_bool;
    } value;
} data_t;   //数据结构体

// 函数声明
void my_onenet_init(void);
void onenet_log_thread(void *param);
void onenet_send_data(data_t* data);
void onenet_send_thread(void* param);
void onenet_receive_thread(void* param);
void onenet_rec_data(void);
void my_onenet_start();  //线程启动函数



#endif // MY_ONENET_H
