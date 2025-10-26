#include <stdio.h>
#include <stdlib.h>  // 添加这个用于malloc/free
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "plat_time.h"

static const char *TAG = "time_esp32";

// 倒计时器结构体
struct countdown_tmr_t {
    uint64_t end_time_ms;
};

// 获取时间戳ms
uint64_t time_count_ms(void) {
    uint64_t time_ms = esp_timer_get_time() / 1000;
    return time_ms;
}

// 获取时间戳s
uint64_t time_count(void) {
    uint64_t time_ms = esp_timer_get_time() / 1000000;
    return time_ms;
}

// 延时ms
void time_delay_ms(uint32_t m_sec) {
    vTaskDelay(m_sec / portTICK_PERIOD_MS);
}

// 延时s
void time_delay(uint32_t sec) {
    vTaskDelay(sec * 1000 / portTICK_PERIOD_MS);
}


// 获取日期时间（仅用于日志）
uint64_t time_get_date(int* year, int* month, int* day, int* hour, int* min, int* sec, int* ms) {
    *year = 0;
    *month = 0;
    *day = 0;
    *hour = 0;
    *min = 0;
    *sec = 0;
    *ms = 0;
    return 0;
}

// 启动倒计时器
handle_t countdown_start(uint32_t ms) {
    struct countdown_tmr_t *tmr = (struct countdown_tmr_t *)malloc(sizeof(struct countdown_tmr_t));
    if (tmr) {
        tmr->end_time_ms = time_count_ms() + ms;
        ESP_LOGD(TAG, "Countdown started: %u ms", ms);
    }
    return (handle_t)tmr;
}

// 重设倒计时器
void countdown_set(handle_t handle, uint32_t new_ms) {
    struct countdown_tmr_t *tmr = (struct countdown_tmr_t *)handle;
    if (tmr) {
        tmr->end_time_ms = time_count_ms() + new_ms;
        ESP_LOGD(TAG, "Countdown reset: %u ms", new_ms);
    }
}

// 获取倒计时器剩余时间
uint32_t countdown_left(handle_t handle) {
    struct countdown_tmr_t *tmr = (struct countdown_tmr_t *)handle;
    if (!tmr) return 0;
    
    uint64_t current = time_count_ms();
    if (current >= tmr->end_time_ms) {
        return 0;
    }
    return (uint32_t)(tmr->end_time_ms - current);
}

// 检查倒计时器是否超时
uint32_t countdown_is_expired(handle_t handle) {
    struct countdown_tmr_t *tmr = (struct countdown_tmr_t *)handle;
    if (!tmr) return 1;
    
    return (time_count_ms() >= tmr->end_time_ms) ? 1 : 0;
}

// 停止倒计时器并释放资源
void countdown_stop(handle_t handle) {
    if (handle) {
        ESP_LOGD(TAG, "Countdown stopped");
        free((void*)handle);  // 直接使用标准C库的free
    }
}
