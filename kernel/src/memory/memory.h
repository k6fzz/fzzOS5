#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <libk/kstring.h>

#include <pmm.h>
#include "vmm.h"
//#include "bitmap.h"


#define MM_PAGE_SIZE 4096

//Higher Half Data Offset     0xFFFF800000000000
//Higher Half Code Offset     0xFFFFFFFF80000000UL
//Higher Half Heap Offset     0xFFFFFFFFC0000000UL



static inline uint64_t hh_data_to_phys(uint64_t addr)
{
    return addr - 0xFFFF800000000000;
}

static inline uint64_t phys_to_hh_data(uint64_t addr)
{
    return 0xFFFF800000000000 + addr;
}

static inline uint64_t hh_code_to_phys(uint64_t addr)
{
    return addr - 0xFFFFFFFF80000000;
}

static inline uint64_t phys_to_hh_code(uint64_t addr)
{
    return 0xFFFFFFFF80000000 + addr;
}

//static inline bool get_la57()
//{
//    uint64_t cr4;
//
//    asm volatile("mov %%cr4, %0" : "=rax"(cr4));
//
//    return (cr4 >> 12) & 1;
//}


