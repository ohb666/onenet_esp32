/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file main.c
 * @brief Thing model example
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "common.h"
#include "dev_cardmgr.h"
#include "err_def.h"
#include "log.h"
#include "aiot_tm_api.h"
#include "tm_user.h"

#ifndef PRODUCT_ID
#define PRODUCT_ID ""
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME ""
#endif

#ifndef ACCESS_KEY
#define ACCESS_KEY ""
#endif

/** token Effective time，The default is 2030.12*/
#define TM_EXPIRE_TIME 1924833600

#include <signal.h> // 添加信号处理头文件
// 定义退出标志（volatile确保可见性，sig_atomic_t确保原子性）
static volatile sig_atomic_t exit_flag = 0;

int main(int argc, char *argv[]) {
  AIOT_ASSERT(!IS_EMPTY(PRODUCT_ID));
  AIOT_ASSERT(!IS_EMPTY(DEVICE_NAME));
  AIOT_ASSERT(!IS_EMPTY(ACCESS_KEY));

  int ret = 0;
  int timeout_ms = 60 * 1000;

  /** Device Login*/
  ret =
      tm_login(PRODUCT_ID, DEVICE_NAME, ACCESS_KEY, TM_EXPIRE_TIME, timeout_ms);
  CHECK_EXPR_GOTO(ERR_OK != ret, _END, "ThingModel login failed!");
  logi("ThingModel login ok");

  while (!exit_flag) {
    if (0 != (ret = tm_step(200))) {
      loge("ThingModel tm_step failed,ret is %d", ret);
      break;
    }
  }

_END:
  /** device deregistration*/
  tm_logout(3000);
  return 0;
}
