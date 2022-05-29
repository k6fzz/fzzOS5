#include "liballoc.h"
#include "memory.h"
#include "pmm.h"
#include "vmm.h"

uint64_t* kernel_heap_top;

extern struct PageTable* kernel_cr3;

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
    
}

void* liballoc_alloc(size_t count)
{
    for(int i = 0; i < count; i++)
    {
        vmm_map_page(kernel_cr3, ((uint64_t)kernel_heap_top + 4096), (uint64_t)pmm_allocpage(), PTE_PRESENT | PTE_READWRITE);
        kernel_heap_top += 4096;
    }
    return (void*)(phys_to_hh_data((uint64_t)kernel_heap_top));
}