#include "serial.h"
#include "../../io/io.h"


uint64_t serial_init(uint16_t port)
{
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x00);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);

    if(inb(port + 0) != 0xAE)
    {
        return 1;
    }

    outb(port + 4, 0x0F);
    return 0;
}

uint64_t serial_received(uint16_t port)
{
    return inb(port + 5) & 1;
}

uint8_t serial_read(uint16_t port)
{
    while(serial_received(port) == 0);

    return inb(port);
}

uint64_t serial_is_tx_empty(uint16_t port)
{
    return inb(port + 5) & 0x20;
}

void serial_write(uint16_t port, uint8_t data)
{
    while(serial_is_tx_empty(port) == 0);
    outb(port, data);
}