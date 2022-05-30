#pragma once

#include <stddef.h>
#include <stdint.h>

#define SERIAL_PORT1 0x3F8

uint64_t serial_init(uint16_t port);

extern char serial_buffer[256];

uint8_t serial_read(uint16_t port);
void serial_write(uint16_t port, uint8_t data);
void serial_write_str(uint16_t port, uint8_t* data);