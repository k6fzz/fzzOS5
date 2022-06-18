#pragma once

#include <stddef.h>
#include <stdint.h>

#include <keyboard.h>

#define ASCII_BS 0x08       //Backspace
#define ASCII_HT 0x09       //Horizontal Tab
#define ASCII_LF 0x0A       //Line Feed
#define ASCII_VT 0x0B       //Vertical Tab
#define ASCII_FF 0x0C       //Form Feed
#define ASCII_CR 0x0D       //Carriage Return

struct console_buffer
{
    uint8_t* buffer;
    uint64_t buffer_size;
    uint64_t index;
};

void putchar(uint8_t c);

void console_init();
void console_exec();

void console_input(KEY_INFO_t key);