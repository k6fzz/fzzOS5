#pragma once

#include <stdint.h>
#include <stddef.h>

typedef size_t uint64_t;

void* memset(void* s, uint64_t c, size_t len);
//void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* str1, const void* str2, size_t n);
