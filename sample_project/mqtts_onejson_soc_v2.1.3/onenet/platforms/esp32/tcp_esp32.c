#include <stdio.h>
#include <stdlib.h>  // 添加这个用于malloc/free
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "plat_tcp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

static const char *TAG = "TCP_ESP32";

//建立TCP连接
handle_t plat_tcp_connect(const uint8_t *host, uint16_t port, uint32_t timeout_ms)
{
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // 创建TCP套接字
    if (client_socket < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    // 设置服务器地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);  // 使用传入的port参数
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0)
    {
        ESP_LOGE(TAG, "Invalid server address");
        close(client_socket);
        return -1;
    }

    // 设置连接超时
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // 尝试连接
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "Connection failed: errno %d", errno);
        close(client_socket);
        return -1;
    }

    ESP_LOGI(TAG, "TCP connected to %s:%d", host, port);
    return (handle_t)client_socket;  // 转换并返回句柄
}

//TCP发送数据
int32_t plat_tcp_send(handle_t handle, void *buf, uint32_t len, uint32_t timeout_ms)
{
    if (handle < 0 || buf == NULL || len == 0) {
        ESP_LOGE(TAG, "Invalid parameters for send");
        return -1;
    }

    int client_socket = (int)handle;

    // 设置发送超时
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    // 发送数据
    int bytes_sent = send(client_socket, buf, len, MSG_NOSIGNAL);

    if (bytes_sent < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ESP_LOGW(TAG, "Send timeout");
            return 0; // 超时
        } else {
            ESP_LOGE(TAG, "Send failed: errno %d", errno);
            return -1; // 错误
        }
    }

    ESP_LOGD(TAG, "Sent %d bytes successfully", bytes_sent);
    return bytes_sent; // 返回实际发送的字节数
}

//TCP接收数据
int32_t plat_tcp_recv(handle_t handle, void *buf, uint32_t len, uint32_t timeout_ms)
{
    if (handle < 0 || buf == NULL || len == 0) {
        ESP_LOGE(TAG, "Invalid parameters for recv");
        return -1;
    }

    int client_socket = (int)handle;

    // 设置接收超时
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // 接收数据
    int bytes_received = recv(client_socket, buf, len, 0);

    if (bytes_received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ESP_LOGW(TAG, "Receive timeout");
            return 0; // 超时
        } else {
            ESP_LOGE(TAG, "Receive failed: errno %d", errno);
            return -1; // 错误
        }
    } else if (bytes_received == 0) {
        ESP_LOGW(TAG, "Connection closed by peer");
        return -1; // 连接关闭
    }

    ESP_LOGD(TAG, "Received %d bytes successfully", bytes_received);
    return bytes_received; // 返回实际接收的字节数
}

//TCP断开连接
int32_t plat_tcp_disconnect(handle_t handle)
{
    if (handle < 0) {
        ESP_LOGE(TAG, "Invalid socket handle");
        return -1;
    }

    int client_socket = (int)handle;

    // 关闭socket连接
    int result = close(client_socket);

    if (result < 0) {
        ESP_LOGE(TAG, "Close socket failed: errno %d", errno);
        return -1;
    }

    ESP_LOGI(TAG, "TCP connection closed successfully");
    return 0; // 成功返回0
}