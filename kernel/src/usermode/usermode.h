#pragma once

#include <stddef.h>
#include <stdint.h>

void user_init();
void user_enter(void* func, uint8_t* stack);

