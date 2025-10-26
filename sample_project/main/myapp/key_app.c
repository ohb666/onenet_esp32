#include "key_app.h"

void key_app_init(void)
{
    key_init();
}

uint8_t key_val,key_dowm,key_up,key_old;

void key_app_run(void)
{
    key_val = key_read();
    key_dowm = key_val & (~key_old);
    key_up = (~key_val) & key_old;
    key_old = key_val;
    if(key_dowm == 1)
    {
        printf("key down\r\n");
    }
}