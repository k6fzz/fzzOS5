#include "vmm.h"
#include "pmm.h"
#include "memory.h"
#include "../libk/kstring.h"
#include "../util/printf.h"

extern uint8_t* _start_of_kernel;
extern uint8_t* _end_of_kernel;
struct PageTable* RootPageDirectory = {0};


struct PageTable* vmm_create_page_table()
{
    void* page = pmm_allocpage();   //get a page
    if(page == NULL) return NULL;   //If it's null, we can't continue

    memset((uint8_t*)page, 0, 4096);    //clear the page
    return (struct PageTable*)phys_to_hh_data((uint64_t)page);
}

void vmm_init()
{
    printf("VMM Init\n");

    //RootPageDirectory = vmm_create_page_table();

    uint64_t* PML4 = (uint64_t*)read_cr3();
    printf("CR3: %p\n", (uint64_t)PML4);
    
    struct PageTable* newPageTable = vmm_create_page_table();

    newPageTable->entry[0] = 0x01;

    printf("%p\n", newPageTable);
    printf("%p\n", newPageTable->entry[0]);
    
    
    //printf("PML4[0] = %p\n", (uint64_t)PML4[0]);

    //uint64_t* PDPTE = (uint64_t*)((PML4[0] >> 12) * 4096);
    
    //printf("PML3[0] = %p\n", PDPTE[0]);

    //uint64_t* PDE = (uint64_t*)((PDPTE[0] >> 12) * 4096);

    //printf("PML2[0] = %p\n", PDE[0]);

    //uint64_t* PTE = (uint64_t*)((PDE[0] >> 12) * 4096);

    //printf("PML1[0] = %p\n", PTE[1]);

    //printf("Root Page Directory at: 0x%p\n", RootPageDirectory);

    

}

/*
void vmm_init()
{
    printf("VMM Init \n");

    RootPageDirectory = vmm_create_page_table();
    printf("PML4 at: %p\n", RootPageDirectory);
    
    vmm_map_page(RootPageDirectory, 0x1000, 0xae73734652f000, 0);

}






void vmm_map_page(struct PageTable* rootpagetable, uint64_t physical, uint64_t virtual, uint64_t flags)
{
    uint64_t virt_addr = virtual;

    virt_addr >>= 12;
    uint64_t index1 = virt_addr & 0x1ff;
    virt_addr >>= 9;
    uint64_t index2 = virt_addr & 0x1ff;
    virt_addr >>= 9;
    uint64_t index3 = virt_addr & 0x1ff;
    virt_addr >>= 9;
    uint64_t index4 = virt_addr & 0x1ff;
    
    if(rootpagetable->entry[index4] = 0x00)
    {
        rootpagetable->entry[index4]=(uint64_t)vmm_create_page_table();
    }
    else
    {

    }



    //uint64_t index4 = (virtual & ((uint64_t)0x1ff << 39));
    //uint64_t index3 = (virtual & ((uint64_t)0x1ff << 30) >> 30);
    //uint64_t index2 = (virtual & ((uint64_t)0x1ff << 21) >> 21);
    //uint64_t index1 = (virtual & ((uint64_t)0x1ff << 12) >> 12);
   
    printf("Indexes: %p  %p  %p  %p\n", index4, index3, index2, index1);
    

    return;
}

*/