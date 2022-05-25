#pragma once

#include <stddef.h>
#include <stdint.h>

uint64_t serial_init(uint16_t port);

uint8_t serial_read(uint16_t port);
void serial_write(uint16_t port, uint8_t data);