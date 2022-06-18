#include "ustar.h"

uint64_t oct2bin(uint8_t* str, uint16_t size)
{
    uint64_t n = 0;
    uint8_t* c = str;
    while (size-- > 0)
    {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

