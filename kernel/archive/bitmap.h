#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct pmm_bitmap_t
{
    uint8_t* address;
    uint64_t size;    
};

bool getBitmap(struct pmm_bitmap_t* bitmap, uint64_t index);
bool setBitmap(struct pmm_bitmap_t* bitmap, uint64_t index, bool value);