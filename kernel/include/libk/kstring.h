#pragma once

#include <stdint.h>
#include <stddef.h>

typedef size_t uint64_t;

//void* memset(void* s, uint64_t c, size_t len);
//void* memcpy(void* dest, const void* src, size_t n);
//int memcmp(const void* str1, const void* str2, size_t n);


static inline void* memset(void* s, uint64_t c, uint64_t len)
{
    unsigned char* p=(uint8_t*)s;
    while(len--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}

static inline int memcmp(const void* str1, const void* str2, size_t n)
{
    if(!n)
        return 0;

    while(--n && *(uint8_t*)str1 == *(uint8_t*)str2)
    {
        str1 = (uint8_t*)str1 + 1;
        str2 = (uint8_t*)str2 + 1;
    }

    return (*((uint8_t*)str1) - *((uint8_t*)str2));
}

static inline uint64_t strlen(const char* str)
{
    uint64_t count = 0;
    while(*str != 0)
    {
        count++;
        str++;
    }
    return count;
}