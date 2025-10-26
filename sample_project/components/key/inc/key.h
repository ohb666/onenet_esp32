#ifndef KEY_H
#define KEY_H


#include "esp_log.h"
#include "driver/gpio.h"

#define KEY_GPIO_NUM 0

uint8_t key_read(void);
void key_init(void);

#endif // DEBUG