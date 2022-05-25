#include "liballoc.h"
#include "pmm.h"

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
    pmm_freepages(page, count);
}

void* liballoc_alloc(size_t count)
{
    pmm_allocpages(count);
}