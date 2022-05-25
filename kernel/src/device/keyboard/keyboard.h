#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint8_t scancode;
    uint8_t ascii;
}KEY_INFO_t;

//uint8_t kbd_translate(uint8_t scancode, uint8_t uppercase);

void kbd_init();
void kbd_handle();
void kbd_set_target(void* handler);