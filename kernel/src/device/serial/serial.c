#include <serial.h>
#include <io.h>

char serial_buffer[256];

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

void serial_write_str(uint16_t port, char* data)
{
    uint8_t i = 0;
    while(data[i] != 0)
    {
        serial_write(port, data[i]);
        //if(data[i] == '\n') serial_write(port, '\r');
        i++;
    }
}