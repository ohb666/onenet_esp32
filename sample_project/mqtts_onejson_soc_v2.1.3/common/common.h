/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file        common.h
 * @brief       通用宏定义头文件
 * @details 提供了一系列常用的宏定义，包括内存管理、断言检查、错误处理等工具宏
 * @version     v1.0.0
 * @author      IoT SDK Team
 * @date        2024-01-01
 *
 * @note        1. 所有宏都使用do-while(0)结构确保安全
 *              2. 内存相关宏会自动处理错误情况
 *              3. 断言宏会导致程序死循环
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#include "err_def.h"
#include "log.h"

/**
 * @brief 断言检查宏
 * @param x 要检查的表达式
 * @note 当断言失败时，会打印错误日志并进入死循环
 * @example AIOT_ASSERT(ptr != NULL);
 */
#ifndef AIOT_ASSERT
#define AIOT_ASSERT(x)                   \
  do {                                   \
    if (!(x)) {                          \
      loge("Assertion failed '%s'", #x); \
      while (1) {                        \
      }                                  \
    }                                    \
  } while (0)
#endif

/**
 * @brief 条件检查并跳转宏
 * @param expr 要检查的条件表达式
 * @param label 跳转标签
 * @param fmt... 错误日志格式字符串和参数
 * @note 当expr为真时，记录错误日志并跳转到指定标签
 * @example CHECK_EXPR_GOTO(ret < 0, cleanup, "Operation failed: %d", ret);
 */
#ifndef CHECK_EXPR_GOTO
#define CHECK_EXPR_GOTO(expr, label, fmt...) \
  do {                                       \
    if (expr) {                              \
      loge(fmt);                             \
      goto label;                            \
    }                                        \
  } while (0)
#endif


#ifndef CHECK_EXPR_RET
#define CHECK_EXPR_RET(expr, ret, ...) \
  do {                                 \
    if (expr) {                        \
      loge(__VA_ARGS__);          \
      return (ret);                    \
    }                                  \
  } while (0)
#endif
/**
 * @brief 安全内存分配宏
 * @param ptr 要分配内存的指针变量
 * @param n 要分配的字节数
 * @note 分配失败时会记录错误日志并进入死循环
 *        分配的内存会自动初始化为0
 * @example SAFE_ALLOC(buffer, 1024);
 */
#ifndef SAFE_ALLOC
#define SAFE_ALLOC(ptr, n)              \
  do {                                  \
    ptr = malloc(n);                    \
    if (!ptr) {                         \
      loge("malloc failed '%s'", #ptr); \
      while (1) {                       \
      }                                 \
    }                                   \
    memset(ptr, '\0', n);               \
  } while (0)
#endif

/**
 * @brief 安全内存重分配宏
 * @param ptr 要重新分配内存的指针变量
 * @param n 要重新分配的字节数
 * @note 重分配失败时会记录错误日志并进入死循环
 * @example SAFE_REALLOC(buffer, 2048);
 */
#ifndef SAFE_REALLOC
#define SAFE_REALLOC(ptr, n)             \
  ({                                     \
    ptr = realloc(ptr, n);               \
    if (!ptr) {                          \
      loge("realloc failed '%s'", #ptr); \
      while (1) {                        \
      }                                  \
    }                                    \
    ptr;                                 \
  })
#endif

/**
 * @brief 安全字符串复制宏
 * @param str 源字符串
 * @param n 要复制的长度
 * @return 复制后的字符串指针（需使用SAFE_FREE释放）
 */
#ifndef SAFE_STRDUP
#define SAFE_STRDUP(str, n)       \
  ({                              \
    char *dup_str = NULL;         \
    if (!IS_EMPTY(str)) {         \
      SAFE_ALLOC(dup_str, n + 1); \
      memcpy(dup_str, str, n);    \
      dup_str[n] = '\0';          \
    }                             \
    dup_str;                      \
  })
#endif


/**
 * @brief 安全内存释放宏
 * @param ptr 要释放内存的指针变量
 * @note 释放后会自动将指针置为NULL
 * @example SAFE_FREE(buffer);
 */
#ifndef SAFE_FREE
#define SAFE_FREE(ptr) \
  do {                 \
    if (ptr) {         \
      free(ptr);       \
      ptr = NULL;      \
    }                  \
  } while (0)
#endif

/**
 * @brief 未使用变量标记宏
 * @param X 未使用的变量
 * @note 用于消除编译器关于未使用变量的警告
 * @example UNUSED(argc);
 */
#ifndef UNUSED
#define UNUSED(X) (void)X
#endif

/**
 * @brief 范围检查宏
 * @param _start 范围起始值
 * @param n 要检查的值
 * @param _end 范围结束值
 * @return 1-在范围内，0-不在范围内
 * @example if (IN_RANGE(0, value, 100)) {...}
 */
#ifndef IN_RANGE
#define IN_RANGE(_start, n, _end) ((n >= _start && n <= _end) ? 1 : 0)
#endif

/**
 * @brief 空字符串检查宏
 * @param str 要检查的字符串
 * @return 1-空字符串，0-非空字符串
 * @note 仅检查第一个字符是否为'\0'
 * @example if (IS_EMPTY(name)) {...}
 */
#define IS_EMPTY(str) ((str) && (str)[0] == '\0')

// 定义一个宏，用于将整数转换为字符串
#define STRINGIFY(x) #x
#define INT_TO_STRING(x) STRINGIFY(x)

/**
 * @brief 控制子设备功能是否启用的宏
 * @details 子设备功能启用条件：
 *          1. 已定义 SDK_USE_MQTTS（启用 MQTTS 协议支持）
 *          2. 已定义 CONFIG_TM_GATEWAY 且其值为 1（启用物模型网关功能）
 */
#define ENABLE_GATEWAY_SUBDEV \
  defined SDK_USE_MQTTS &&    \
      (defined(CONFIG_TM_GATEWAY) && (CONFIG_TM_GATEWAY == 1))

#endif