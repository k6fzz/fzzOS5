#include "bitmap.h"

bool getBitmap(struct pmm_bitmap_t* bitmap, uint64_t index)
{
    if(index > bitmap->size * 8)
        return false;

    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t indexer = 0b10000000 >>bitIndex;
    if ((bitmap->address[byteIndex] & indexer) > 0)
        return true;
    
    return false;
}

bool setBitmap(struct pmm_bitmap_t* bitmap, uint64_t index, bool value)
{
    if(index > bitmap->size * 8) return false;

    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t indexer = 0b10000000 >>bitIndex;
    bitmap->address[byteIndex] &= ~indexer;

    if(value)
        bitmap->address[byteIndex] |= indexer;

    return true;
}