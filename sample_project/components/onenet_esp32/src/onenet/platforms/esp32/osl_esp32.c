/**
 * @file osl_esp32.c
 * @brief ESP32 Operating System Layer implementation
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "esp_random.h"
#include "plat_osl.h"

char* g_server_ip = NULL;
char* g_server_port = NULL;

void *osl_malloc(size_t size) {
    return malloc(size);
}

void *osl_calloc(size_t num, size_t size) {
    return calloc(num, size);
}

void osl_free(void *ptr) {
    free(ptr);
}

void *osl_memcpy(void *dst, const void *src, size_t n) {
    return memcpy(dst, src, n);
}

void *osl_memmove(void *dst, const void *src, size_t n) {
    return memmove(dst, src, n);
}

void *osl_memset(void *dst, int32_t val, size_t n) {
    return memset(dst, val, n);
}

uint8_t *osl_strdup(const uint8_t *s) {
    return (uint8_t *)strdup((const char *)s);
}

uint8_t *osl_strndup(const uint8_t *s, size_t n) {
    return (uint8_t *)strndup((const char *)s, n);
}

uint8_t *osl_strcpy(uint8_t *s1, const uint8_t *s2) {
    return (uint8_t *)strcpy((char *)s1, (const char *)s2);
}

uint8_t *osl_strncpy(uint8_t *s1, const uint8_t *s2, size_t n) {
    return (uint8_t *)strncpy((char *)s1, (const char *)s2, n);
}

uint8_t *osl_strcat(uint8_t *dst, const uint8_t *src) {
    return (uint8_t *)strcat((char *)dst, (const char *)src);
}

uint8_t *osl_strstr(const uint8_t *s1, const uint8_t *s2) {
    return (uint8_t *)strstr((const char *)s1, (const char *)s2);
}

uint32_t osl_strlen(const uint8_t *s) {
    return (uint32_t)strlen((const char *)s);
}

int32_t osl_strcmp(const uint8_t *s1, const uint8_t *s2) {
    return strcmp((const char *)s1, (const char *)s2);
}

int32_t osl_strncmp(const uint8_t *s1, const uint8_t *s2, size_t n) {
    return strncmp((const char *)s1, (const char *)s2, n);
}

int32_t osl_sprintf(uint8_t *str, const uint8_t *format, ...) {
    va_list args;
    va_start(args, format);
    int32_t ret = vsprintf((char *)str, (const char *)format, args);
    va_end(args);
    return ret;
}

int32_t osl_sscanf(const uint8_t *str, const uint8_t *format, ...) {
    va_list args;
    va_start(args, format);
    int32_t ret = vsscanf((const char *)str, (const char *)format, args);
    va_end(args);
    return ret;
}

void osl_assert(boolean expression) {
    if (!expression) {
        abort();
    }
}

int32_t osl_get_random(unsigned char *buf, size_t len) {
    esp_fill_random(buf, len);
    return 0;
}

int32_t osl_atoi(const uint8_t *nptr) {
    return atoi((const char *)nptr);
}

int32_t osl_rand(int32_t min, int32_t max) {
    if (min >= max) {
        return min;
    }
    uint32_t range = max - min + 1;
    return min + (esp_random() % range);
}

uint8_t *osl_random_string(uint8_t *buf, int len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < len; i++) {
        buf[i] = charset[esp_random() % (sizeof(charset) - 1)];
    }
    buf[len] = '\0';
    return buf;
}

int32_t module_init(void *arg, void* callback) {
    return 0;
}

int32_t module_deinit(void) {
    return 0;
}
