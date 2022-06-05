#pragma once

#include <stdint.h>
#include <stddef.h>

//#define ALIGN_DOWN(addr, align)	    ((addr) & ~((align)-1))
//#define ALIGN_UP(addr, align)	    (((addr) + (align)-1) & ~((align)-1))
#define PAGE_SIZE 4096
#define KB_TO_PAGE(x) (((x)*1024)/PAGE_SIZE)

void pmm_init();
void* pmm_allocpage();
void pmm_freepage(void* page);

void* pmm_allocpages(uint64_t count);
uint64_t pmm_freepages(void* page, uint64_t count);

uint64_t pmm_get_free_memory();
uint64_t pmm_get_total_memory();

struct pmm_info
{
    uint64_t totalmem;          //Size of memory in bytes
    uint64_t usedpages;         //Number of used pages
    uint64_t totalpages;        //Number of total pages
    uint8_t* bitmap;            //Address of Bitmap
    uint64_t bitmap_size;       //Size of Bitmap, in bytes 
};

struct pmm_to_vmm_info
{
    uint64_t totalmem;
    uint64_t fb_base;
    uint64_t fb_size;
};


/*
struct bitmap_t
{
    uint8_t* address;
    uint64_t size;          //size in bytes    
};
*/

