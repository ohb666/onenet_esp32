#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include <sys/socket.h>	//提供套接字
#include <netdb.h>//域名和服务器转换 ->ip
#include <errno.h>//C语言标准错误处理
#include <arpa/inet.h>//地址格式和字节序转换
#include "my_wifi_app.h"
#include "esp_timer.h"
#include "ringbuffer.h"


typedef enum {
    SOCKET_DISCONNECTED = 0,
    SOCKET_CONNECTED = 1,
    SOCKET_RECONNECTING = 2
} socket_state_t;

void socket_connected_thread(void *pvParameters);   // TCP 连接线程
void socket_send_thread(void *pvParameters); //TCP发送线程
void socket_rec_thread(void *pvParameters); //TCP接收线程
void socket_herat_thread(void *pvParameters); //TCP心跳监测线程
int my_socket_init(void); //Socket初始化函数
void my_socket_start(void); //启动所有socket线程

#endif //MY_SOCKET_H