#include "keyboard.h"

#include <stddef.h>
#include <stdint.h>

#include <io.h>
#include <kprintf.h>
#include "../console/console.h"
#include "../../interrupts/idt.h"
#include "../console/console.h"

#define KBD_LEFT_SHIFT  0x2A
#define KBD_RIGHT_SHIFT 0x36
#define KBD_ENTER       0x1C
#define KBD_BACKSPACE   0x0E
#define KBD_SPACEBAR    0x39
#define KBD_ESCAPE      0x01

static const uint8_t kbd_table[58] = 
{
    0  , 0  , '1', '2', '3', '4', '5', '6', 
    '7', '8', '9', '0', '-', '=', 0  , 0  ,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', 0  , 0  , 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'','`', 0  , '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',' , '.', '/', 0  , '*', 
    0  , ' ' 
};

uint8_t lshift_status;
uint8_t rshift_status;

uint8_t kbd_buffer[256];
uint64_t kbd_buffer_index;

static uint8_t kbd_active = 0;
static void (*kbd_handler)(KEY_INFO_t);


void kbd_init()
{

}

void kbd_set_target(void* handler)
{
    kbd_handler = handler;
    kbd_active = 1;
}

uint8_t kbd_translate(uint8_t scancode, uint8_t uppercase)
{
    if(scancode > 58) return 0;

    if(uppercase) return kbd_table[scancode] - 32;

    return kbd_table[scancode];
}

void kbd_handle()
{
    uint8_t data;
    KEY_INFO_t key_info;

    if(!kbd_active) return;

    data = inb(0x60);

    uint8_t buffer[20];

    switch(data)
    {
        case KBD_LEFT_SHIFT:
            lshift_status = 1;
            return;
        case KBD_RIGHT_SHIFT:
            rshift_status = 1;
            return;
        case KBD_LEFT_SHIFT + 0x80:
            lshift_status = 0;
            return;
        case KBD_RIGHT_SHIFT + 0x80:
            rshift_status = 0;
            return;
        case KBD_ESCAPE:
            printf("%d\n", pit_ticks);
            //snprintf(buffer, 20, "%d", pit_ticks);
            return;
    }

    
    key_info.ascii = kbd_translate(data, lshift_status | rshift_status);
    key_info.scancode = data;

    kbd_handler(key_info);

}