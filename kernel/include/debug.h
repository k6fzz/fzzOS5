#pragma once

#include <stdarg.h>
#include <serial.h>
#include <kprintf.h>



#ifdef __DEBUG__
    #define DEBUG_MSG(...) serial_printf(SERIAL_PORT1, __VA_ARGS__)
#endif