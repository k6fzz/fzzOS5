#pragma once

#include <stdint.h>
#include <stddef.h>

//#define ALIGN_DOWN(addr, align)	    ((addr) & ~((align)-1))
//#define ALIGN_UP(addr, align)	    (((addr) + (align)-1) & ~((align)-1))


void pmm_init();
void* pmm_allocpage();
void pmm_freepage(void* page);

void* pmm_allocpages(uint64_t count);
uint64_t pmm_freepages(void* page, uint64_t count);

uint64_t pmm_get_free_memory();
uint64_t pmm_get_total_memory();


/*
struct bitmap_t
{
    uint8_t* address;
    uint64_t size;          //size in bytes    
};
*/

