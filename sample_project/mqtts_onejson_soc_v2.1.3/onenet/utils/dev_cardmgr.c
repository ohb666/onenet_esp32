#include "dev_cardmgr.h"

#include "cJSON.h"
#include "common.h"

struct card_srv_ctx_s {
  char *sn;  // 设备SN
  char *mac; // 设备MAC

  char *imei;       // 设备IMEI
  char *iccid;      // 设备ICCID
  char *imsi;       // 设备IMSI
  char *msisdn;     // 设备MSISDN
  card_type_e type; // 设备网络类型
};

#if 0
#ifdef SDK_USE_MQTTS

const char *generate_user_clientid(char *buf, uint32_t buf_len,
                                   const char *device_name) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);

  uint32_t offset = 0;
  memset(buf, 0, buf_len);

  offset += snprintf(buf, buf_len, "%s", device_name);

#if defined(CONFIG_CARDMGR_MODE) && (IN_RANGE(1, CONFIG_CARDMGR_MODE, 2))
#if defined(DEV_NETWORK_TYPE)

#if (DEV_NETWORK_TYPE == 0)

  if (!IS_EMPTY(DEV_SN) || !IS_EMPTY(DEV_MAC)) {
    offset += snprintf(buf + offset, buf_len - offset, "?network=%d",
                       DEV_NETWORK_TYPE);

    // 添加设备序列号（SN）字段
    if (!IS_EMPTY(DEV_SN)) {
      offset += snprintf(buf + offset, buf_len - offset, "&sn=%s", DEV_SN);
    }
    // 添加设备MAC地址字段
    if (!IS_EMPTY(DEV_MAC)) {
      offset += snprintf(buf + offset, buf_len - offset, "&mac=%s", DEV_MAC);
    }
  }

#elif IN_RANGE(1, DEV_NETWORK_TYPE, 4)

  // imei 不能为空；iccid、imsi、msisnd不能全为空
  AIOT_ASSERT(!IS_EMPTY(DEV_IMEI));
  AIOT_ASSERT(!IS_EMPTY(DEV_ICCID) || !IS_EMPTY(DEV_IMSI) ||
              !IS_EMPTY(DEV_MSISDN));
  // 构建JSON内容
  offset += snprintf(buf + offset, buf_len - offset, "?network=%d&imei=%s",
                     DEV_NETWORK_TYPE, DEV_IMEI);

  if (!IS_EMPTY(DEV_ICCID)) {
    offset += snprintf(buf + offset, buf_len - offset, "&iccid=%s", DEV_ICCID);
  }

  else if (!IS_EMPTY(DEV_IMSI)) {
    offset += snprintf(buf + offset, buf_len - offset, "&imsi=%s", DEV_IMSI);
  }

  else if (!IS_EMPTY(DEV_MSISDN)) {
    offset +=
        snprintf(buf + offset, buf_len - offset, "&msisdn=%s", DEV_MSISDN);
  }

#endif

#endif
#endif

  return buf;
}

uint32_t cardmgr_json_msg(void *root, const char *imei,
                                 const char *iccid, const char *imsi,
                                 const char *msisdn) {
#define ADD_DEVICE_INFO(field, value)                                          \
  if (!IS_EMPTY(value)) {                                                      \
    cJSON_AddStringToObject(root, field, value);                               \
  }

  AIOT_ASSERT(root != NULL);

  cJSON_AddNumberToObject(root, "network", DEV_NETWORK_TYPE);

#if (DEV_NETWORK_TYPE == 0)
  {
    ADD_DEVICE_INFO("sn", DEV_SN);
    ADD_DEVICE_INFO("mac", DEV_MAC);
  }
#elif (IN_RANGE(1, DEV_NETWORK_TYPE, 4))
  {
    ADD_DEVICE_INFO("imei", imei);
    ADD_DEVICE_INFO("iccid", iccid);
    ADD_DEVICE_INFO("imsi", imsi);
    ADD_DEVICE_INFO("msisdn", msisdn);
  }
#endif
}

uint32_t generate_cmp_payload(char *buf, uint32_t buf_len, uint32_t id) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);

  char id_str[16] = {0};
  char *temp = NULL;

  memset(buf, '\0', buf_len);

#if defined(DEV_NETWORK_TYPE) && (DEV_NETWORK_TYPE == 0)
  if (IS_EMPTY(DEV_SN) && IS_EMPTY(DEV_MAC)) {
    return 0;
  }
#endif

  snprintf(id_str, sizeof(id_str), "%d", id);

  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "id", id_str);

  cJSON *params = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "params", params);

#if 0
  cJSON_AddNumberToObject(params, "network", DEV_NETWORK_TYPE);

// 提取公共逻辑：添加设备信息到params
#define ADD_DEVICE_INFO(field, value)                                          \
  if (!IS_EMPTY(value)) {                                                      \
    cJSON_AddStringToObject(params, field, value);                             \
  }

#if (DEV_NETWORK_TYPE == 0)
  {
    ADD_DEVICE_INFO("sn", DEV_SN);
    ADD_DEVICE_INFO("mac", DEV_MAC);
  }
#elif (IN_RANGE(1, DEV_NETWORK_TYPE, 4))
  {
    ADD_DEVICE_INFO("imei", DEV_IMEI);
    ADD_DEVICE_INFO("iccid", DEV_ICCID);
    ADD_DEVICE_INFO("imsi", DEV_IMSI);
    ADD_DEVICE_INFO("msisdn", DEV_MSISDN);
  }
#endif

#else
  cardmgr_json_msg(params, DEV_IMEI, DEV_ICCID, DEV_IMSI, DEV_MSISDN);
#endif

  temp = cJSON_PrintUnformatted(root);
  if (temp == NULL) {
    // 错误处理：释放资源并返回空字符串
    cJSON_Delete(root);
    return 0;
  }

  cJSON_Delete(root);

  AIOT_ASSERT(strlen(temp) < buf_len);
  memcpy(buf, temp, strlen(temp));

  // 释放cJSON_PrintUnformatted分配的内存
  free(temp);
  logd("%s", buf);
  return strlen(buf);
}
#endif

#ifdef SDK_USE_HTTPS

const char *generate_sim_info(char *buf, uint32_t buf_len) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);

#define ADD_FIELD(field, value)                                                \
  if (!IS_EMPTY(value)) {                                                      \
    offset += snprintf(buf + offset, buf_len - offset, ",\"%s\":\"%s\"",       \
                       field, value);                                          \
  }
  uint32_t offset = 0;

  memset(buf, '\0', buf_len);

#if defined(CONFIG_CARDMGR_MODE) && CONFIG_CARDMGR_MODE == 1

#if defined(DEV_NETWORK_TYPE)

#if (DEV_NETWORK_TYPE == 0)

  if (!IS_EMPTY(DEV_SN) || !IS_EMPTY(DEV_MAC)) {
    offset += snprintf(buf + offset, buf_len - offset, ",\"network\":%d",
                       DEV_NETWORK_TYPE);
    ADD_FIELD("sn", DEV_SN);
    ADD_FIELD("mac", DEV_MAC);
  }

#elif IN_RANGE(1, DEV_NETWORK_TYPE, 4)

  // imei 不能为空；iccid、imsi、msisnd不能全为空
  AIOT_ASSERT(!IS_EMPTY(DEV_IMEI));
  AIOT_ASSERT(!IS_EMPTY(DEV_ICCID) || !IS_EMPTY(DEV_IMSI) ||
              !IS_EMPTY(DEV_MSISDN));
  // 构建JSON内容
  offset += snprintf(buf + offset, buf_len - offset, ",\"network\":%d",
                     DEV_NETWORK_TYPE);
  ADD_FIELD("imei", DEV_IMEI);

  ADD_FIELD("iccid", DEV_ICCID);
  ADD_FIELD("imsi", DEV_IMSI);
  ADD_FIELD("msisdn", DEV_MSISDN);

#endif

#endif
#endif

  return buf;
}
#endif
#endif

/**
 * 创建一个空的卡服务上下文（无卡设备）
 * @param sn 设备序列号
 * @param mac 设备MAC地址
 * @return 成功返回卡服务上下文指针，失败返回NULL
 * @note 默认卡类型为CARD_TYPE_NONE
 */
cardmgr_ctx_t *create_cardless_service_context(const char *sn,
                                               const char *mac) {
  if (sn == NULL || mac == NULL) {
    return NULL; // 参数检查
  }

  cardmgr_ctx_t *card_ctx = NULL;
  SAFE_ALLOC(card_ctx, sizeof(cardmgr_ctx_t));

  if (sn != NULL && sn[0] != '\0') {
    card_ctx->sn = strdup(sn);
  }

  card_ctx->type = CARD_TYPE_NONE;

  if (mac != NULL && mac[0] != '\0') {
    card_ctx->mac = strdup(mac);
  }

  return card_ctx;
}

/**
 * 创建一个带卡的卡服务上下文（有卡设备）
 * @param imei 设备IMEI
 * @param iccid 卡ICCID
 * @param imsi 卡IMSI
 * @param msisdn 卡MSISDN
 * @param type 卡类型
 * @return 成功返回卡服务上下文指针，失败返回NULL
 */
cardmgr_ctx_t *create_card_bundled_service_context(const char *imei,
                                                   const char *iccid,
                                                   const char *imsi,
                                                   const char *msisdn,
                                                   card_type_e type) {
  AIOT_ASSERT(imei != NULL);

  AIOT_ASSERT((IS_EMPTY(iccid) && IS_EMPTY(imsi) && IS_EMPTY(msisdn)) == false);

  AIOT_ASSERT(type != CARD_TYPE_NONE);

  cardmgr_ctx_t *card_ctx = NULL;
  SAFE_ALLOC(card_ctx, sizeof(cardmgr_ctx_t));
  card_ctx->type = type;

  card_ctx->imei = strdup(imei);

  if (!IS_EMPTY(iccid)) {
    card_ctx->iccid = strdup(iccid);
  }

  if (!IS_EMPTY(imsi)) {
    card_ctx->imsi = strdup(imsi);
  }

  if (!IS_EMPTY(msisdn)) {
    card_ctx->msisdn = strdup(msisdn);
  }
  return card_ctx;
}

static void destroy_card_service_context(cardmgr_ctx_t *card_ctx) {
  if (card_ctx != NULL) {

    logd("Destroying card service context...");

    SAFE_FREE(card_ctx->sn);
    SAFE_FREE(card_ctx->mac);

    SAFE_FREE(card_ctx->imei);
    SAFE_FREE(card_ctx->iccid);
    SAFE_FREE(card_ctx->imsi);
    SAFE_FREE(card_ctx->msisdn);
    SAFE_FREE(card_ctx);
  }
}

static const char *
generate_cardmgr_msgstr_for_login(char *buf, uint32_t buf_len,
                                  const cardmgr_ctx_t *card_ctx) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);
  AIOT_ASSERT(card_ctx != NULL);

  uint32_t offset = 0;

#ifdef SDK_USE_MQTTS

  if (card_ctx->type == CARD_TYPE_NONE) {
    if (!IS_EMPTY(card_ctx->sn) || !IS_EMPTY(card_ctx->mac)) {
      offset += snprintf(buf + offset, buf_len - offset, "?network=%d",
                         card_ctx->type);

      // 添加设备序列号（SN）字段
      if (!IS_EMPTY(card_ctx->sn)) {
        offset +=
            snprintf(buf + offset, buf_len - offset, "&sn=%s", card_ctx->sn);
      }
      // 添加设备MAC地址字段
      if (!IS_EMPTY(card_ctx->mac)) {
        offset +=
            snprintf(buf + offset, buf_len - offset, "&mac=%s", card_ctx->mac);
      }
    }
  } else {
    // 构建JSON内容
    offset += snprintf(buf + offset, buf_len - offset, "?network=%d&imei=%s",
                       card_ctx->type, card_ctx->imei);

    if (!IS_EMPTY(card_ctx->iccid)) {
      offset += snprintf(buf + offset, buf_len - offset, "&iccid=%s",
                         card_ctx->iccid);
    }

    else if (!IS_EMPTY(card_ctx->imsi)) {
      offset +=
          snprintf(buf + offset, buf_len - offset, "&imsi=%s", card_ctx->imsi);
    }

    else if (!IS_EMPTY(card_ctx->msisdn)) {
      offset += snprintf(buf + offset, buf_len - offset, "&msisdn=%s",
                         card_ctx->msisdn);
    }
  }

#elif defined(SDK_USE_HTTPS)
#elif defined(SDK_USE_NBIOT)

  AIOT_ASSERT(card_ctx->type != CARD_TYPE_NONE);
  AIOT_ASSERT(card_ctx->imei != NULL);

  offset += snprintf(buf + offset, buf_len - offset, "%s;%d", card_ctx->iccid,
                     card_ctx->type);
#else
#error "Unsupported SDK mode"
#endif
  buf[offset] = '\0'; // 确保字符串以NULL结尾
  logd("Generated cardmgr login message: %s", buf);
  return buf;
}

static void cardmgr_json_msg(void *root, cardmgr_ctx_t *card_ctx) {
  AIOT_ASSERT(root != NULL);
  AIOT_ASSERT(card_ctx != NULL);

#define ADD_DEVICE_INFO(field, value)                                          \
  if (!IS_EMPTY(value)) {                                                      \
    cJSON_AddStringToObject(root, field, value);                               \
  }

  AIOT_ASSERT(root != NULL);

  cJSON_AddNumberToObject(root, "network", card_ctx->type);

  if (card_ctx->type == CARD_TYPE_NONE) {
    if (!IS_EMPTY(card_ctx->sn) || !IS_EMPTY(card_ctx->mac)) {
      ADD_DEVICE_INFO("sn", card_ctx->sn);
      ADD_DEVICE_INFO("mac", card_ctx->mac);
    }
  } else {
    ADD_DEVICE_INFO("imei", card_ctx->imei);
    ADD_DEVICE_INFO("iccid", card_ctx->iccid);
    ADD_DEVICE_INFO("imsi", card_ctx->imsi);
    ADD_DEVICE_INFO("msisdn", card_ctx->msisdn);
  }
}

extern int32_t get_post_id(void);
static const char *
generate_cardmgr_msgstr_for_topic(char *buf, uint32_t buf_len,
                                  const cardmgr_ctx_t *card_ctx) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);
  AIOT_ASSERT(card_ctx != NULL);

  char id_str[16] = {0};
  char *payload_str = NULL;

  snprintf(id_str, sizeof(id_str), "%d", get_post_id());

  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "id", id_str);

  cJSON *params = cJSON_CreateObject();
  cJSON_AddItemToObject(root, "params", params);

  cardmgr_json_msg(params, card_ctx);

  payload_str = cJSON_PrintUnformatted(root);

  cJSON_Delete(root);

  uint32_t payload_len = strlen(payload_str);
  AIOT_ASSERT(payload_len < buf_len);

  memcpy(buf, payload_str, payload_len);

  // 释放cJSON_PrintUnformatted分配的内存
  SAFE_FREE(payload_str);
  buf[payload_len] = '\0'; // 确保字符串以NULL结尾
  logd("Generated cardmgr topic message: %s", buf);

  return buf;
}

/*
 ********************************************************************************************
 * @brief 生成机卡协同消息字符串
 ********************************************************************************************
 */

static cardmgr_ctx_t *s_cardmgr_ctx = NULL;

void cardmgr_ctx_init_for_nocard(const char *sn, const char *mac) {
  s_cardmgr_ctx = create_cardless_service_context(sn, mac);
}

void cardmgr_ctx_init_for_card(const char *imei, const char *iccid,
                               const char *imsi, const char *msisdn,
                               card_type_e type) {
  s_cardmgr_ctx =
      create_card_bundled_service_context(imei, iccid, imsi, msisdn, type);
}

void cardmgr_ctx_destroy() { destroy_card_service_context(s_cardmgr_ctx); }

const char *generate_cardmgr_msg(char *buf, uint32_t buf_len, int type) {
  AIOT_ASSERT(buf != NULL);
  AIOT_ASSERT(buf_len > 0);
  AIOT_ASSERT(s_cardmgr_ctx != NULL);

  AIOT_ASSERT(
      IN_RANGE(CARDMGR_MSG_MODE_LOGIN, type, CARDMGR_MSG_MODE_SUBDEVICE_LOGIN));

  switch (type) {
  case CARDMGR_MSG_MODE_LOGIN:
    return generate_cardmgr_msgstr_for_login(buf, buf_len, s_cardmgr_ctx);
  case CARDMGR_MSG_MODE_TOPIC:
    return generate_cardmgr_msgstr_for_topic(buf, buf_len, s_cardmgr_ctx);
  default:
    return buf;
  }
}

void generate_cardmgr_msgstr_by_cjson(void *cjson) {
  AIOT_ASSERT(cjson != NULL);
  AIOT_ASSERT(s_cardmgr_ctx != NULL);

  cardmgr_json_msg(cjson, s_cardmgr_ctx);
}
