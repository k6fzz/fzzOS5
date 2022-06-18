#include <stdarg.h>
#include <stdint.h>

#include <kprintf.h>
#include <boot.h>
#include <serial.h>
#include "../device/console/console.h"

void _putchar(char character)
{
    
    putchar(character);

    //term_write(&character, 1);
    
    //serial_write(0x3F8, character);
}

void serial_printf(uint16_t port, const char* format, ...)
{
    char buffer[64];

    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, 64, format, argptr);
    va_end(argptr);

    serial_write_str(port, buffer);
}