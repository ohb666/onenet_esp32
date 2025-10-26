/* ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "sdkconfig.h"
#include "rom/ets_sys.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include "my_wifi.h"

/* The examples use WiFi configuration that you can set via 'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY
#define EXAMPLE_ESP_WIFI_AP_SSID   CONFIG_ESP_WIFI_AP_SSID
#define EXAMPLE_ESP_WIFI_AP_PASS   CONFIG_ESP_WIFI_AP_PASSWORD
#define EXAMPLE_MAX_STA_CONN       CONFIG_MAX_STA_CONN
#define EXAMPLE_IP_ADDR            CONFIG_SERVER_IP
#define EXAMPLE_ESP_WIFI_AP_CHANNEL CONFIG_ESP_WIFI_AP_CHANNEL

static const char *TAG = "my wifi";

static int s_retry_num = 0;
static bool s_reconnect = true;

static bool wifi_connected = false;

WiFi_Connect_Status wifi_connected_already(void)
{
    WiFi_Connect_Status status;
    if (true == wifi_connected) {
        status = WIFI_STATUS_CONNECTED_OK;
    } else {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            status = WIFI_STATUS_CONNECTING;
        } else {
            status = WIFI_STATUS_CONNECTED_FAILED;
        }
    }
    return status;
}

void wifi_print_status(void)
{
    wifi_ap_record_t ap_info;
    esp_netif_ip_info_t ip_info;
    
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGI(TAG, "Connected to AP: %s, RSSI: %d", ap_info.ssid, ap_info.rssi);
    } else {
        ESP_LOGI(TAG, "Not connected to any AP");
    }
    
    esp_netif_t* sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (sta_netif && esp_netif_get_ip_info(sta_netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&ip_info.ip));
        ESP_LOGI(TAG, "STA Gateway: " IPSTR, IP2STR(&ip_info.gw));
        ESP_LOGI(TAG, "STA Netmask: " IPSTR, IP2STR(&ip_info.netmask));
        ESP_LOGI(TAG, "WiFi connected status: %s", wifi_connected ? "true" : "false");
    } else {
        ESP_LOGI(TAG, "STA has no IP address");
        ESP_LOGI(TAG, "WiFi connected status: %s", wifi_connected ? "true" : "false");
    }
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WiFi STA started");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "WiFi connected to AP");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGI(TAG, "WiFi disconnected from AP, reason: %d", event->reason);
        
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retry to connect to AP, retry count: %d", s_retry_num);
        } else {
            ESP_LOGE(TAG, "Connect to AP failed after %d retries", s_retry_num);
        }
        wifi_connected = false;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&event->ip_info.gw));
        ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&event->ip_info.netmask));
        s_retry_num = 0;
        wifi_connected = true;
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" joined AP, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" left AP, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(esp_netif_t *wifi_netif)
{
    if (strcmp(EXAMPLE_IP_ADDR, "192.168.4.1"))
    {
        esp_netif_ip_info_t ip;
        memset(&ip, 0 , sizeof(esp_netif_ip_info_t));
        ip.ip.addr = ipaddr_addr(EXAMPLE_IP_ADDR);
        ip.gw.addr = ipaddr_addr(EXAMPLE_IP_ADDR);
        ip.netmask.addr = ipaddr_addr("255.255.255.0");
        ESP_ERROR_CHECK(esp_netif_dhcps_stop(wifi_netif));
        ESP_ERROR_CHECK(esp_netif_set_ip_info(wifi_netif, &ip));
        ESP_ERROR_CHECK(esp_netif_dhcps_start(wifi_netif));
    }
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));
    strncpy((char*)wifi_config.ap.ssid, EXAMPLE_ESP_WIFI_AP_SSID, sizeof(wifi_config.ap.ssid) - 1);
    wifi_config.ap.ssid_len = strlen((char*)wifi_config.ap.ssid);
    strncpy((char*)wifi_config.ap.password, EXAMPLE_ESP_WIFI_AP_PASS, sizeof(wifi_config.ap.password) - 1);
    wifi_config.ap.max_connection = EXAMPLE_MAX_STA_CONN;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    if (strlen(EXAMPLE_ESP_WIFI_AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    if (strlen(EXAMPLE_ESP_WIFI_AP_CHANNEL)) {
        int channel;
        sscanf(EXAMPLE_ESP_WIFI_AP_CHANNEL, "%d", &channel);
        wifi_config.ap.channel = channel;
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_AP_SSID, EXAMPLE_ESP_WIFI_AP_PASS);
}

void wifi_init_sta(void)
{
    wifi_config_t wifi_config = {
        .sta = {
            .scan_method = WIFI_FAST_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
            .threshold.authmode = WIFI_AUTH_OPEN,
        },
    };
    
    strncpy((char*)wifi_config.sta.ssid, EXAMPLE_ESP_WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, EXAMPLE_ESP_WIFI_PASS, sizeof(wifi_config.sta.password) - 1);
    
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(TAG, "STA configured - SSID: %s", wifi_config.sta.ssid);
}

void app_wifi_task(void *arg)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Initialize networking stack
    ESP_ERROR_CHECK(esp_netif_init());
    
    // Create default event loop needed by the main app
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_mode_t mode = WIFI_MODE_NULL;
    esp_netif_t *sta_netif = NULL;
    esp_netif_t *ap_netif = NULL;

    // Determine WiFi mode and create corresponding network interfaces
    if (strlen(EXAMPLE_ESP_WIFI_AP_SSID) && strlen(EXAMPLE_ESP_WIFI_SSID)) {
        mode = WIFI_MODE_APSTA;
        ap_netif = esp_netif_create_default_wifi_ap();
        sta_netif = esp_netif_create_default_wifi_sta();
        ESP_LOGI(TAG, "WiFi mode: AP+STA");
    } else if (strlen(EXAMPLE_ESP_WIFI_AP_SSID)) {
        mode = WIFI_MODE_AP;
        ap_netif = esp_netif_create_default_wifi_ap();
        ESP_LOGI(TAG, "WiFi mode: AP");
    } else if (strlen(EXAMPLE_ESP_WIFI_SSID)) {
        mode = WIFI_MODE_STA;
        sta_netif = esp_netif_create_default_wifi_sta();
        ESP_LOGI(TAG, "WiFi mode: STA");
    }

    if (mode == WIFI_MODE_NULL) {
        ESP_LOGW(TAG, "Neither AP or STA have been configured. WiFi will be off.");
        vTaskDelete(NULL);
        return;
    }

    // Initialize WiFi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    // Set WiFi mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM)); // Use RAM storage to avoid NVS issues
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    // Configure AP mode (if enabled)
    if (mode & WIFI_MODE_AP) {
        wifi_init_softap(ap_netif);
    }

    // Configure STA mode (if enabled)
    if (mode & WIFI_MODE_STA) {
        wifi_init_sta();
    }
    
    // Start WiFi
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "WiFi initialization completed");
    
    vTaskDelete(NULL);
}

void app_wifi_start(void)
{
    BaseType_t ret_val = xTaskCreatePinnedToCore(app_wifi_task, "Wifi Init Task", 8 * 1024, NULL, 1, NULL, 0);
    if (pdPASS != ret_val) {
        ESP_LOGE(TAG, "Failed to create WiFi task");
    }
}
