cmake_minimum_required(VERSION 3.10) 
project(mqtts_onejson_soc C)

include_directories(
    common
    3rd/cJSON
    onenet/utils
    onenet/protocols/mqtt
    onenet/protocols/mqtt/paho-mqtt
    3rd/paho-mqtt
    onenet/security/tls
    3rd/wolfssl/wolfssl-3.15.3
    onenet/security/tls/wolfssl
    onenet/platforms/include
    onenet/tm
    examples/things_model/
)
set(SRC_FILES
    common/log.c
    common/utils.c
    common/slist.c
    3rd/cJSON/cJSON.c
    onenet/protocols/mqtt/paho-mqtt/mqtt_client.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/coding.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hash.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/hmac.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/logging.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/md5.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/misc.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/sha256.c
    onenet/security/tls/tls.c
    onenet/platforms/linux/osl_linux.c
    onenet/platforms/linux/tcp_linux.c
    onenet/platforms/linux/time_linux.c
    onenet/platforms/linux/udp_linux.c
    onenet/utils/dev_token.c
    onenet/utils/dev_cardmgr.c
    onenet/tm/aiot_tm_api.c
    onenet/tm/tm_data.c
    onenet/tm/tm_onejson.c
    examples/things_model/tm_user.c
    examples/things_model/main.c
    onenet/tm/tm_mqtt.c
    onenet/tm/tm_subdev.c
    onenet/tm/dev_discov.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/aes.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/asn.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/integer.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/random.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/rsa.c
    3rd/wolfssl/wolfssl-3.15.3/wolfcrypt/src/wc_port.c
    3rd/wolfssl/wolfssl-3.15.3/src/internal.c
    3rd/wolfssl/wolfssl-3.15.3/src/keys.c
    3rd/wolfssl/wolfssl-3.15.3/src/ssl.c
    3rd/wolfssl/wolfssl-3.15.3/src/tls.c
    3rd/wolfssl/wolfssl-3.15.3/src/wolfio.c
    onenet/security/tls/tls.c
    3rd/paho-mqtt/MQTTConnectClient.c
    3rd/paho-mqtt/MQTTDeserializePublish.c
    3rd/paho-mqtt/MQTTFormat.c
    3rd/paho-mqtt/MQTTPacket.c
    3rd/paho-mqtt/MQTTSerializePublish.c
    3rd/paho-mqtt/MQTTSubscribeClient.c
    3rd/paho-mqtt/MQTTUnsubscribeClient.c
    onenet/protocols/mqtt/paho-mqtt/mqtt_client.c
)

add_definitions(
    -w
    -g
    -Wno-incompatible-function-pointer-types
    -DPLAT_HAVE_STDINT=1
    -DSDK_PAYLOAD_LEN=4096
    -DSDK_SEND_BUF_LEN=4096
    -DSDK_RECV_BUF_LEN=4096
    -DSDK_REQUEST_TIMEOUT=4096
    -DSDK_ACCESS_LIFE_TIME=120
    -DLOG_LEVEL=LOG_LEVEL_DEBUG
    -DCONFIG_PLAT_ARCH_64BIT=1
    -DCONFIG_TM_GATEWAY=0
    -DSDK_USE_MQTTS
    -DCONFIG_CARDMGR_MODE=0
    -DCONFIG_NETWORK_TLS=0
    -DIOT_MQTT_SERVER_ADDR_TLS="mqttstls.heclouds.com"
    -DIOT_MQTT_SERVER_PORT_TLS=8883
    -DIOT_MQTT_SERVER_ADDR="mqtts.heclouds.com"
    -DIOT_MQTT_SERVER_PORT=1883
)




add_executable(mqtts_onejson_soc ${SRC_FILES})
target_link_libraries(mqtts_onejson_soc pthread)
