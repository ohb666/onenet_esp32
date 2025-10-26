idf_component_register(
    SRC_DIRS
        "src/common"
        "src/3rd/cJSON"
        "src/3rd/paho-mqtt"
        "src/3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src"
        "src/onenet/platforms/esp32"
        "src/onenet/utils"
        "src/onenet/tm"
        "src/onenet/protocols/mqtt/paho-mqtt"

    INCLUDE_DIRS
        "inc"
        "src/common"
        "src/3rd/cJSON"
        "src/3rd/paho-mqtt"
        "src/3rd/wolfssl/wolfssl-3.15.3"
        "src/onenet/security/tls"
        "src/onenet/security/tls/wolfssl"
        "src/onenet/platforms/include"
        "src/onenet/utils"
        "src/onenet/tm"
        "src/onenet/protocols/mqtt"
        "src/onenet/protocols/mqtt/paho-mqtt"

    REQUIRES driver lwip nvs_flash esp_timer
)

# 添加 OneNET SDK 所需的宏定义
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    PLAT_HAVE_STDINT=1
    SDK_PAYLOAD_LEN=4096
    SDK_SEND_BUF_LEN=4096
    SDK_RECV_BUF_LEN=4096
    SDK_REQUEST_TIMEOUT=4096
    SDK_ACCESS_LIFE_TIME=120
    LOG_LEVEL=LOG_LEVEL_DEBUG
    CONFIG_TM_GATEWAY=0
    SDK_USE_MQTTS
    CONFIG_CARDMGR_MODE=0
    CONFIG_NETWORK_TLS=0
    IOT_MQTT_SERVER_ADDR="mqtts.heclouds.com"
    IOT_MQTT_SERVER_PORT=1883
)

# 禁用一些警告
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-format
    -Wno-pointer-sign
    -Wno-incompatible-pointer-types
    -Wno-missing-braces
    -Wno-unused-label
)
