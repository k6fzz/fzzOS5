#include "stty.h"
#include <serial.h>



void stty_sendcmd(uint16_t cmd)
{
    serial_write(SERIAL_PORT1, 0x1B);
    serial_write(SERIAL_PORT1, 0x5B);
    serial_write(SERIAL_PORT1, '[');
    serial_write(SERIAL_PORT1, 'H');
    serial_write(SERIAL_PORT1, '~');
}