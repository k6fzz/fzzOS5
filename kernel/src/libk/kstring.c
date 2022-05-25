#include "kstring.h"

void* memset(void* s, uint64_t c, uint64_t len)
{
    unsigned char* p=s;
    while(len--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}

int memcmp(const void* str1, const void* str2, size_t n)
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