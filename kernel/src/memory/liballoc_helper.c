#include <liballoc.h>
#include "memory.h"
#include <pmm.h>
#include "vmm.h"

uint64_t kernel_heap_start = 0xFFFFFFFFC0000000;

uint64_t kernel_heap_top = 0xFFFFFFFFC0000000;

extern struct PageTable* RootPageDirectory;



int liballoc_lock()
{
    asm("cli");
    return 0;
}


int liballoc_unlock()
{
    asm("sti");
    return 0;
}

int liballoc_free(void* page, size_t count)
{
    return 0;
}

void* liballoc_alloc(size_t count)
{
    for(uint64_t i = 0; i < count; i++)
    {
        vmm_map_4Kpage(RootPageDirectory, (kernel_heap_top + 4096), (uint64_t)pmm_allocpage(), PTE_PRESENT | PTE_READWRITE);
        kernel_heap_top += 4096;
    }
    return (void*)(phys_to_hh_data((uint64_t)kernel_heap_top));
}