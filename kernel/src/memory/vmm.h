#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pmm.h>

#define PTE_PRESENT             1
#define PTE_READWRITE           2
#define PTE_USER_SUPERVISOR     4
#define PTE_WRITE_THROUGH       8
#define PTE_CACHE_DISABLED      16
#define PTE_ACCESSED            32
#define PTE_DIRTY               64
#define PTE_PAT                 128
#define PTE_PAGESIZE            128
#define PTE_GLOBAL              256

struct PageTable
{
    uint64_t entry[512];
}__attribute__((packed));



extern struct PageTable* RootPageDirectory;

void vmm_init();
void vmm_map_4Kpage(struct PageTable* pagetable, uint64_t virtual, uint64_t physical, uint64_t flags);
void vmm_unmap_page(struct PageTable* pagetable, uint64_t virtual);

uint8_t vmm_get_index(uint64_t addr, uint8_t level);



enum vmm_flags
{
    present         = (1 << 0),
    readwrite       = (1 << 1),
    usersuper       = (1 << 2),
    writethrough    = (1 << 3),
    cachedisable    = (1 << 4),
    accessed        = (1 << 5),
    dirty           = (1 << 6),
    pat             = (1 << 7),
    global          = (1 << 8),
};