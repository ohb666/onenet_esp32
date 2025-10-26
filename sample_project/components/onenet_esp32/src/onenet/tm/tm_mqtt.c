/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file tm_mqtt.c
 * @brief MQTT protocol process with Thing Model data
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "tm_mqtt.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "dev_cardmgr.h"
#include "err_def.h"
#include "mqtt_api.h"
#include "plat_osl.h"
#include "plat_time.h"

/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

#if defined(CONFIG_NETWORK_TLS) && CONFIG_NETWORK_TLS == 1

static const uint8_t *g_tm_cert =
    (const uint8_t *)"-----BEGIN CERTIFICATE-----\r\n"
                     "MIIDOzCCAiOgAwIBAgIJAPCCNfxANtVEMA0GCSqGSIb3DQEBCwUAMDQxC"
                     "zAJBgNV\r\n"
                     "BAYTAkNOMQ4wDAYDVQQKDAVDTUlPVDEVMBMGA1UEAwwMT25lTkVUIE1RV"
                     "FRTMB4X\r\n"
                     "DTE5MDUyOTAxMDkyOFoXDTQ5MDUyMTAxMDkyOFowNDELMAkGA1UEBhMCQ"
                     "04xDjAM\r\n"
                     "BgNVBAoMBUNNSU9UMRUwEwYDVQQDDAxPbmVORVQgTVFUVFMwggEiMA0GC"
                     "SqGSIb3\r\n"
                     "DQEBAQUAA4IBDwAwggEKAoIBAQC/"
                     "VvJ6lGWfy9PKdXKBdzY83OERB35AJhu+9jkx\r\n"
                     "5d4SOtZScTe93Xw9TSVRKrFwu5muGgPusyAlbQnFlZoTJBZY/"
                     "745MG6aeli6plpR\r\n"
                     "r93G6qVN5VLoXAkvqKslLZlj6wXy70/"
                     "e0GC0oMFzqSP0AY74icANk8dUFB2Q8usS\r\n"
                     "UseRafNBcYfqACzF/Wa+Fu/"
                     "upBGwtl7wDLYZdCm3KNjZZZstvVB5DWGnqNX9HkTl\r\n"
                     "U9NBMS/7yph3XYU3mJqUZxryb8pHLVHazarNRppx1aoNroi+5/t3Fx/"
                     "gEa6a5PoP\r\n"
                     "ouH35DbykmzvVE67GUGpAfZZtEFE1e0E/"
                     "6IB84PE00llvy3pAgMBAAGjUDBOMB0G\r\n"
                     "A1UdDgQWBBTTi/"
                     "q1F2iabqlS7yEoX1rbOsz5GDAfBgNVHSMEGDAWgBTTi/q1F2ia\r\n"
                     "bqlS7yEoX1rbOsz5GDAMBgNVHRMEBTADAQH/"
                     "MA0GCSqGSIb3DQEBCwUAA4IBAQAL\r\n"
                     "aqJ2FgcKLBBHJ8VeNSuGV2cxVYH1JIaHnzL6SlE5q7MYVg+"
                     "Ofbs2PRlTiWGMazC7\r\n"
                     "q5RKVj9zj0z/8i3ScWrWXFmyp85ZHfuo/"
                     "DeK6HcbEXJEOfPDvyMPuhVBTzuBIRJb\r\n"
                     "41M27NdIVCdxP6562n6Vp0gbE8kN10q+ksw8YBoLFP0D1da7D5WnSV+"
                     "nwEIP+F4a\r\n"
                     "3ZX80bNt6tRj9XY0gM68mI60WXrF/"
                     "qYL+NUz+D3Lw9bgDSXxpSN8JGYBR85BxBvR\r\n"
                     "NNAhsJJ3yoAvbPUQ4m8J/"
                     "CoVKKgcWymS1pvEHmF47pgzbbjm5bdthlIx+swdiGFa\r\n"
                     "WzdhzTYwVkxBaU+xf/2w\r\n"
                     "-----END CERTIFICATE-----";

#else

static const uint8_t *g_tm_cert = NULL;
#endif

#define THING_MODEL_SUBED_TOPIC "$sys/%s/%s/thing/#"
/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/

typedef struct tm_mqtt_obj_s {
  void *client;
  struct mqtt_param_t mqtt_param;
  uint8_t *subed_topic;
  uint8_t *cmp_topic;
  tm_mqtt_message_cb recv_cb;
} tm_mqtt_obj_t;

/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/
tm_mqtt_obj_t *g_mqtt_obj = NULL;

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/

static void tm_mqtt_message_arrived(void *arg, const uint8_t *topic,
                                    struct mqtt_message_t *message) {
  g_mqtt_obj->recv_cb(topic, message->payload, message->payload_len);
}

static uint8_t *tm_topic_construct(const uint8_t *format, const uint8_t *pid,
                                   const uint8_t *dev_name) {
  uint32_t topic_len = 0;
  uint8_t *topic = NULL;

  topic_len = osl_strlen(format) + osl_strlen(pid) + osl_strlen(dev_name) - 3;

  if (NULL != (topic = osl_malloc(topic_len))) {
    osl_memset(topic, 0, topic_len);
    osl_sprintf(topic, format, pid, dev_name);
  }

  return topic;
}

void tm_mqtt_init(tm_mqtt_message_cb msg_cb) {
  SAFE_ALLOC(g_mqtt_obj, sizeof(tm_mqtt_obj_t));

  g_mqtt_obj->mqtt_param.send_buf_len = SDK_SEND_BUF_LEN;
  SAFE_ALLOC(g_mqtt_obj->mqtt_param.send_buf,
             g_mqtt_obj->mqtt_param.send_buf_len);

  g_mqtt_obj->mqtt_param.recv_buf_len = SDK_RECV_BUF_LEN;
  SAFE_ALLOC(g_mqtt_obj->mqtt_param.recv_buf,
             g_mqtt_obj->mqtt_param.recv_buf_len);

  g_mqtt_obj->recv_cb = msg_cb;
}

void tm_mqtt_deinit(void) {
  if (g_mqtt_obj) {
    SAFE_FREE(g_mqtt_obj->mqtt_param.send_buf);
    SAFE_FREE(g_mqtt_obj->mqtt_param.recv_buf);

    SAFE_FREE(g_mqtt_obj);
  }
}

int32_t tm_mqtt_login(const uint8_t *product_id, const uint8_t *dev_name,
                      const uint8_t *dev_token, uint32_t timeout_ms) {
  handle_t cd_hdl = 0;
  int32_t ret = ERR_OK;

  char clientid_buf[128] = {0};
  uint32_t offset = 0;

  if (0 == (cd_hdl = countdown_start(timeout_ms))) {
    return -1;
  }

  offset += snprintf(clientid_buf + offset, sizeof(clientid_buf) - offset, "%s",
                     dev_name);

#if ENABLE_CARDMGR(CARDMGR_MSG_MODE_LOGIN)
  generate_cardmgr_msg(clientid_buf + offset, sizeof(clientid_buf) - offset,CARDMGR_MSG_MODE_LOGIN);
#endif

  g_mqtt_obj->mqtt_param.client_id = clientid_buf;

  logd("client_id:%s", g_mqtt_obj->mqtt_param.client_id);
  g_mqtt_obj->mqtt_param.username = product_id;
  g_mqtt_obj->mqtt_param.password = dev_token;

  g_mqtt_obj->mqtt_param.connect_flag = MQTT_CONNECT_FLAG_CLEAN_SESSION |
                                        MQTT_CONNECT_FLAG_USERNAME |
                                        MQTT_CONNECT_FLAG_PASSWORD;

  g_mqtt_obj->subed_topic = tm_topic_construct(
      (const uint8_t *)THING_MODEL_SUBED_TOPIC, product_id, dev_name);

#if defined(CONFIG_NETWORK_TLS) && CONFIG_NETWORK_TLS == 1
  g_mqtt_obj->client =
      mqtt_connect((const uint8_t *)IOT_MQTT_SERVER_ADDR_TLS,
                   IOT_MQTT_SERVER_PORT_TLS, g_tm_cert, osl_strlen(g_tm_cert),
                   &(g_mqtt_obj->mqtt_param), countdown_left(cd_hdl));

#else
  g_mqtt_obj->client =
      mqtt_connect((const uint8_t *)IOT_MQTT_SERVER_ADDR, IOT_MQTT_SERVER_PORT,
                   NULL, 0, &(g_mqtt_obj->mqtt_param), countdown_left(cd_hdl));
#endif

  CHECK_EXPR_GOTO(NULL == g_mqtt_obj->client, exit, "mqtt connect failed!");

  ret = mqtt_subscribe(g_mqtt_obj->client, g_mqtt_obj->subed_topic, MQTT_QOS0,
                       tm_mqtt_message_arrived, NULL, countdown_left(cd_hdl));

  CHECK_EXPR_GOTO(ret != ERR_OK, exit, "mqtt subscribe failed!");
  logd("subscribe %s success!", g_mqtt_obj->subed_topic);

#if ENABLE_CARDMGR(CARDMGR_MSG_MODE_TOPIC)
  {
    // 订阅

    SAFE_ALLOC(g_mqtt_obj->cmp_topic, 64);
    snprintf(g_mqtt_obj->cmp_topic, 64, "$sys/%s/%s/cmp/#", product_id,
             dev_name);
    int ret =
        mqtt_subscribe(g_mqtt_obj->client, g_mqtt_obj->cmp_topic, MQTT_QOS0,
                       tm_mqtt_message_arrived, NULL, timeout_ms);
    CHECK_EXPR_GOTO(ret != ERR_OK, exit, "mqtt subscribe %s failed!",
                    g_mqtt_obj->cmp_topic);
    logd("subscribe %s success!", g_mqtt_obj->cmp_topic);
  }

#endif
  countdown_stop(cd_hdl);
  return 0;

exit:
  countdown_stop(cd_hdl);
  tm_mqtt_logout(timeout_ms);
  return -1;
}

int32_t tm_mqtt_logout(uint32_t timeout_ms) {
  if (g_mqtt_obj->client) {
    mqtt_disconnect(g_mqtt_obj->client, timeout_ms);
    g_mqtt_obj->client = NULL;
  }
  if (g_mqtt_obj->subed_topic) {
    osl_free(g_mqtt_obj->subed_topic);
    g_mqtt_obj->subed_topic = NULL;
  }

  return 0;
}

int32_t tm_mqtt_send_packet(const uint8_t *topic, uint8_t *payload,
                            uint32_t payload_len, uint32_t timeout_ms) {
  struct mqtt_message_t msg;

  osl_memset(&msg, 0, sizeof(struct mqtt_message_t));
  msg.qos = MQTT_QOS0;
  msg.payload = payload;
  msg.payload_len = payload_len;

  return mqtt_publish(g_mqtt_obj->client, topic, &msg, timeout_ms);
}

int32_t tm_mqtt_step(uint32_t timeout_ms) {
  return mqtt_yield(g_mqtt_obj->client, timeout_ms);
}
