#include <stdarg.h>
#include <stdint.h>

#include <kprintf.h>
#include <boot.h>
#include <serial.h>
#include <libk/kstring.h>
#include "../device/console/console.h"



void _putchar(char character)
{
    
    putchar(character);

    //term_write(&character, 1);
    
    //serial_write(0x3F8, character);
}

void serial_printf(uint16_t port, const char* format, ...)
{
    char buffer[256];

    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, 256, format, argptr);
    va_end(argptr);

    serial_write_str(port, buffer);
}

void bootloader_printf(const char* format, ...)
{
    char buffer[256];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, 256, format, argptr);
    va_end(argptr);

    if(boot_info.type == STIVALE2)
    {

        //stivale_term_write(buffer, strlen(buffer));
    }
    else if(boot_info.type == LIMINE)
    {
  
        //limine_term_write(boot_info.limine_terminal, buffer, strlen(buffer));
    }
}