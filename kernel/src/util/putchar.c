#include "printf.h"
#include "../boot/boot.h"
#include "../device/serial/serial.h"
#include "../device/console/console.h"

void _putchar(char character)
{
    
    putchar(character);

    //term_write(&character, 1);
    
    //serial_write(0x3F8, character);
}