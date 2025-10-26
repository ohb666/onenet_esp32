#include "key.h"

void key_init(void) 
{
    gpio_config_t gpio_struct;
    gpio_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_struct.mode = GPIO_MODE_INPUT;
    gpio_struct.pin_bit_mask = (1ULL << KEY_GPIO_NUM);
    gpio_struct.pull_down_en = 0;
    gpio_struct.pull_up_en = 1;
    gpio_config(&gpio_struct);
}

uint8_t key_read(void)
{
    uint8_t temp=0;
    if(gpio_get_level(KEY_GPIO_NUM)==0)
    {
        temp=1;
    }
    return temp;
}