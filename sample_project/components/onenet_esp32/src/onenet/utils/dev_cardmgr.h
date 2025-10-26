// 机卡协同

#ifndef __DEV_CARD_MANAGER_H__
#define __DEV_CARD_MANAGER_H__

#include "common.h"

#define ENABLE_CARDMGR(mode) \
  defined(CONFIG_CARDMGR_MODE) && (CONFIG_CARDMGR_MODE == mode)

#define CARDMGR_MSG_MODE_LOGIN (1)  // 设备登录时携带机卡信息上报
#define CARDMGR_MSG_MODE_TOPIC (2)  // 使用专门主题上报机卡信息
#define CARDMGR_MSG_MODE_SUBDEVICE_LOGIN (3)  // 子设备登录时上报机卡信息

typedef enum {
  CARD_TYPE_NONE = 0,   // 无卡或不支持蜂窝网络
  CARD_TYPE_MOBILE,     // 移动网络
  CARD_TYPE_TELECOM,    // 电信网络
  CARD_TYPE_UNICOM,     // 联通网络
  CARD_TYPE_BROADCAST,  // 广电网络
} card_type_e;

typedef struct card_srv_ctx_s cardmgr_ctx_t;
#ifdef __cplusplus
extern "C" {
#endif
void cardmgr_ctx_init_for_nocard(const char *sn, const char *mac);
void cardmgr_ctx_init_for_card(const char *imei, const char *iccid,
                               const char *imsi, const char *msisdn,
                               card_type_e type);
void cardmgr_ctx_destroy();

const char *generate_cardmgr_msg(char *buf, uint32_t buf_len, int type);

void generate_cardmgr_msgstr_by_cjson(void *cjson);

#ifndef DEV_NETWORK_TYPE
// 机卡协同相关参数，2025/05/12 add
// 0 - 非蜂窝设备;
// 1 - 使用移动卡设备; 2 - 使用电信卡设备;
// 3 - 使用联通卡设备; 4 - 使用广电卡设备
#define DEV_NETWORK_TYPE (0)
#endif

#if defined(DEV_NETWORK_TYPE)

#if !IN_RANGE(0, DEV_NETWORK_TYPE, 4)
#error "Please define DEV_NETWORK_TYPE as a value between 0 and 4"
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
