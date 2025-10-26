#include "my_socket.h"

#define IP_TARGET "192.168.192.77"  // 替换为你的目标IP地址
#define PORT 8080       //  替换为你的目标端口
#define HEART_TIME 10000    // 心跳包发送间隔，单位毫秒
#define RETRY_COUNT 6    // 重试次数

static const char *TAG = "my_socket";
struct sockaddr_in server_addr;
static  int sock; // 套接字
struct rt_ringbuffer rec_ringbuffer; //接收缓冲区
rt_uint8_t recv_buf[1024];    
socket_state_t my_socket_stat = SOCKET_DISCONNECTED;    //连接标志位
uint8_t count=0; //重试计数器

SemaphoreHandle_t socket_ok_to_send=NULL; // 创建一个信号量
SemaphoreHandle_t socket_ok_to_rec=NULL; // 创建一个信号量
SemaphoreHandle_t socket_ok_to_herat=NULL; // 创建一个信号量

int my_socket_init(void)
{
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // 创建TCP套接字
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    // 尝试连接（server_addr在心跳线程中已初始化）
    int erro_code = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (erro_code < 0)
    {
        ESP_LOGE(TAG, "Socket connect failed: errno %d", errno);
        close(sock);
        sock = -1;
        return -1;
    }

    my_socket_stat = SOCKET_CONNECTED;  // 更新连接状态
    ESP_LOGI(TAG, "Socket connected successfully");
    return sock; // 返回套接字描述符
}

void socket_connected_thread(void *pvParameters)    //TCP连接线程
{
    ESP_LOGI(TAG, "Socket thread waiting for Wi-Fi...");
    socket_ok_to_send = xSemaphoreCreateBinary(); // 创建一个二值信号量
    if(socket_ok_to_send == NULL)
    {
        ESP_LOGE(TAG, "Failed to create socket_ok_to_send semaphore");
        vTaskDelete(NULL);
        return;  // 添加return避免继续执行
    }
    socket_ok_to_rec = xSemaphoreCreateBinary(); // 创建一个二值信号量
    if(socket_ok_to_rec == NULL)
    {
        ESP_LOGE(TAG, "Failed to create socket_ok_to_rec semaphore");
        vTaskDelete(NULL);
        return;  // 添加return避免继续执行
    }
    socket_ok_to_herat= xSemaphoreCreateBinary(); //心跳包信号量
    if(socket_ok_to_herat == NULL)
    {
         ESP_LOGE(TAG, "Failed to create socket_ok_to_heart semaphore");
        vTaskDelete(NULL);
        return;  // 添加return避免继续执行       
    }

    xSemaphoreTake(wifi_ok, portMAX_DELAY);
    ESP_LOGI(TAG, "Wi-Fi is ready, starting socket connection...");

    while ((sock = my_socket_init()) < 0)
    {
        ESP_LOGI(TAG, "Socket connect failed, retrying...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Socket is connected!");
    xSemaphoreGive(socket_ok_to_send);
    xSemaphoreGive(socket_ok_to_rec);
    xSemaphoreGive(socket_ok_to_herat);
    vTaskDelete(NULL);
}

void socket_send_thread(void *pvParameters) //TCP发送线程
{
    ESP_LOGI(TAG, "Socket_send thread waiting for socket...");
    xSemaphoreTake(socket_ok_to_send, portMAX_DELAY);

    while (1)
    {
        if(my_socket_stat == SOCKET_CONNECTED) 
        {
            // uint32_t time_temp = esp_timer_get_time() / 1000000;
            // char *string = NULL;
            // asprintf(&string, "The systick time:%ld", time_temp);

            // // 发送数据
            // int bytes_sent = send(sock, string, strlen(string), 0);
            // if (bytes_sent < 0) {
            //     ESP_LOGE(TAG, "Send failed: errno %d", errno);
            //     my_socket_stat= SOCKET_DISCONNECTED;
            // }

            // free(string);  // 释放动态分配的内存
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


void socket_rec_thread(void *pvParameters) //TCP接收线程
{
    ESP_LOGI(TAG, "Socket_rec thread waiting for socket...");
    xSemaphoreTake(socket_ok_to_rec, portMAX_DELAY);

    while (1)
    {
        if(my_socket_stat == SOCKET_CONNECTED)
        {
            char rec_temp[1024];
            memset(rec_temp, 0, sizeof(rec_temp));

            int bytes_received = recv(sock, rec_temp, sizeof(rec_temp) - 1, 0);
            if (bytes_received > 0)
            {
                rec_temp[bytes_received] = '\0';  // 确保字符串终止符

                // 将接收到的数据放入环形缓冲区
                rt_size_t put_len = rt_ringbuffer_put(&rec_ringbuffer,
                                                    (const rt_uint8_t*)rec_temp,
                                                    bytes_received);

                ESP_LOGI(TAG, "Received %d bytes, put %d bytes into ringbuffer", bytes_received, put_len);

                // 检查缓冲区中是否有数据需要处理
                rt_size_t data_size = rt_ringbuffer_data_len(&rec_ringbuffer);
                if (data_size > 0)
                {
                    // 分配足够的空间来存储数据
                    char *rec_op = (char*)malloc(data_size + 1);
                    if (rec_op != NULL)
                    {
                        memset(rec_op, 0, data_size + 1);
                        // 从环形缓冲区读取数据
                        rt_size_t get_len = rt_ringbuffer_get(&rec_ringbuffer,
                                                            (rt_uint8_t*)rec_op,
                                                            data_size);
                        rec_op[get_len] = '\0';  // 确保字符串终止符

                        ESP_LOGI(TAG, "Received: %s", rec_op);
                        free(rec_op);  // 释放内存
                    }
                    else
                    {
                        ESP_LOGE(TAG, "Memory allocation failed");
                    }
                }
            }
            else if (bytes_received == 0)
            {
                ESP_LOGW(TAG, "Connection closed by peer");
                my_socket_stat = SOCKET_DISCONNECTED;
                close(sock);
                sock = -1;
            }
            else if (bytes_received < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // 超时，继续等待
                    ESP_LOGD(TAG, "recv timeout");
                } else {
                    ESP_LOGE(TAG, "recv error: errno %d", errno);
                    my_socket_stat = SOCKET_DISCONNECTED;
                    close(sock);
                    sock = -1;
                }
            }
        }
        else
        {
            // 连接断开时，减少延迟，更快响应重连
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void socket_herat_thread(void *pvParameters) //TCP心跳加监测
{
    ESP_LOGI(TAG, "Socket_heart thread waiting for socket...");
    xSemaphoreTake(socket_ok_to_herat, portMAX_DELAY);

    // 初始化重连计数器
    count = 0;

    // 初始化server_addr（只需要设置一次）
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP_TARGET, &server_addr.sin_addr) <= 0) {
        ESP_LOGE(TAG, "Invalid server address");
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
        if(my_socket_stat == SOCKET_CONNECTED)
        {
            // 发送心跳包
            const char* heart_buf = "heartbeat\r\n";
            int bytes_sent = send(sock, heart_buf, strlen(heart_buf), MSG_NOSIGNAL);
            if (bytes_sent < 0)
            {
                ESP_LOGE(TAG, "Heartbeat failed: errno %d", errno);
                my_socket_stat = SOCKET_DISCONNECTED;
                count = 0; // 重置计数器
            }
            else
            {
                ESP_LOGD(TAG, "Heartbeat sent successfully");
            }
        }
        else if (my_socket_stat == SOCKET_DISCONNECTED)
        {
            // 检查重试次数
            if(count >= RETRY_COUNT)
            {
                ESP_LOGE(TAG, "Max retry count (%d) reached, giving up", RETRY_COUNT);
                // 可以选择退出任务或进入休眠状态
                vTaskDelay(60000 / portTICK_PERIOD_MS); // 等待1分钟后重新开始
                count = 0; // 重置计数器，重新开始
                continue;
            }

            count++;
            ESP_LOGI(TAG, "Attempting reconnection %d/%d", count, RETRY_COUNT);

            // 关闭旧socket
            if (sock >= 0) {
                close(sock);
                sock = -1;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS); // 等待1秒再重连

            // 尝试重连
            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
            if (sock < 0)
            {
                ESP_LOGE(TAG, "Socket creation failed: errno %d", errno);
                continue; // 继续下一次重试
            }

            int erro_code = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
            if (erro_code < 0)
            {
                ESP_LOGE(TAG, "Connection failed: errno %d", errno);
                close(sock);
                sock = -1;
            }
            else
            {
                ESP_LOGI(TAG, "Reconnection successful!");
                my_socket_stat = SOCKET_CONNECTED;
                count = 0; // 重置计数器
            }
        }
        else if (my_socket_stat == SOCKET_RECONNECTING)
        {
            // 重连中状态，等待连接完成
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(HEART_TIME / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

// 添加线程创建函数
void my_socket_start(void)
{
    // 创建环形缓冲区
    rt_ringbuffer_init(&rec_ringbuffer, recv_buf, sizeof(recv_buf));

    // 创建连接线程
    xTaskCreate(socket_connected_thread, "socket_connected", 4096, NULL, 6, NULL);

    // 创建心跳线程（优先级稍高）
    xTaskCreate(socket_herat_thread, "socket_heart", 4096, NULL, 5, NULL);

    // 创建发送线程
    xTaskCreate(socket_send_thread, "socket_send", 4096, NULL, 4, NULL);

    // 创建接收线程
    xTaskCreate(socket_rec_thread, "socket_rec", 4096, NULL, 4, NULL);
}



