#include "vmm.h"
#include "pmm.h"
#include "memory.h"
#include "../libk/kstring.h"
#include "../util/printf.h"

extern uint8_t* _start_of_kernel, 
    _start_of_text,
    _end_of_text,
    _start_of_rodata,
    _end_of_rodata,
    _start_of_data,
    _end_of_data,
    _start_of_bss,
    _end_of_bss,
    _end_of_kernel;

extern void vmm_flush_tlb(void* vaddr);
extern uint64_t read_cr3();
extern void write_cr3(uint64_t value);
struct PageTable* RootPageDirectory = {0};

struct PageTable* kernel_cr3 = {0};

uint64_t magic = 0xdeadbeef;

struct PageTable* vmm_create_page_table()
{
    void* page = pmm_allocpage();   //get a page
    if(page == NULL) return NULL;   //If it's null, we can't continue

    memset((uint8_t*)page, 0, 4096);    //clear the page
    return (struct PageTable*)phys_to_hh_data((uint64_t)page);
}

static inline struct PageTable* vmm_get_pagemap(struct PageTable* pagemap, uint64_t index, uint64_t flags)
{
    if(pagemap->entry[index] & 1)
    {
        return (struct PageTable*)(pagemap->entry[index] & ~(511));
    }
    else
    {
        pagemap->entry[index] = (uint64_t)pmm_allocpage() | flags;
        return (struct PageTable*)(pagemap->entry[index] & (~511));
    }
}

void vmm_map_page(struct PageTable* pagetable, uint64_t virtual, uint64_t physical, uint64_t flags)
{
    //Get the indices for the virtual page
    uint64_t vaddr = virtual;
    uint64_t index1, index2, index3, index4;
    vaddr >>= 12;
    index1 = vaddr & 0x1ff;
    vaddr >>= 9;
    index2 = vaddr & 0x1ff;
    vaddr >>= 9;
    index3 = vaddr & 0x1ff;
    vaddr >>= 9;
    index4 = vaddr & 0x1ff;

    struct PageTable* PML4;
    struct PageTable* PML3;
    struct PageTable* PML2; 
    struct PageTable* PML1;

    PML4 = pagetable;
    PML3 = vmm_get_pagemap(PML4, index4, flags);
    PML2 = vmm_get_pagemap(PML3, index3, flags);
    PML1 = vmm_get_pagemap(PML2, index2, flags);

    PML1->entry[index1] = physical | flags;

    vmm_flush_tlb((void*)virtual);

}

//returns a physical address for a given virtual address
uint64_t vmm_pagewalk(uint64_t vaddr, uint64_t* cr3)
{
    uint64_t* PML4 = (uint64_t*)(phys_to_hh_data((uint64_t)cr3));
    
    uint64_t index1, index2, index3, index4;
    vaddr >>= 12;
    index1 = vaddr & 0x1ff;
    vaddr >>= 9;
    index2 = vaddr & 0x1ff;
    vaddr >>= 9;
    index3 = vaddr & 0x1ff;
    vaddr >>= 9;
    index4 = vaddr & 0x1ff;
   
    uint64_t* PDPTE = (uint64_t*)((phys_to_hh_data(PML4[index4]) >> 12) * 4096);
    uint64_t* PDE = (uint64_t*)((phys_to_hh_data(PDPTE[index3]) >> 12) * 4096);
    uint64_t* PTE = (uint64_t*)((phys_to_hh_data(PDE[index2]) >> 12) * 4096);
    uint64_t* PD = (uint64_t*)((phys_to_hh_data(PTE[index1]) >> 12) * 4096);

    uint64_t pageaddr = hh_data_to_phys((uint64_t)PD);
    
    printf("%d  %d  %d  %d  %p\n", index4, index3, index2, index1, pageaddr);
    
    
    pageaddr >>= 12;
    pageaddr *= 4096;

    return pageaddr;
}

void vmm_init()
{
    printf("VMM Init\n");

    //RootPageDirectory = vmm_create_page_table();

    kernel_cr3 = (struct PageTable*)read_cr3();
    printf("CR3: %p\n", (uint64_t)kernel_cr3);
    
    //struct PageTable* newPageTable = vmm_create_page_table();

    //newPageTable->entry[0] = 0x01;

    //printf("%p\n", newPageTable);
    //printf("%p\n", newPageTable->entry[0]);
    
    uint64_t kernel_size = (uint64_t)&_end_of_kernel - (uint64_t)&_start_of_kernel;

    printf("Kernel: %p - %p Size: %d\n", &_start_of_kernel, &_end_of_kernel, kernel_size);
    printf("Text:   %p - %p \n", &_start_of_text, &_end_of_text);
    printf("Data:   %p - %p \n", &_start_of_data, &_end_of_data);
    printf("ROData: %p - %p \n", &_start_of_rodata, &_end_of_rodata);
    printf("BSS:    %p - %p \n", &_start_of_bss, &_end_of_bss);

    

    //vmm_pagewalk((uint64_t)&_start_of_kernel, (uint64_t*)read_cr3());
    //vmm_pagewalk((uint64_t)&_end_of_kernel, (uint64_t*)read_cr3());

    //vmm_pagewalk(0xFFFF000000008000, (uint64_t*)read_cr3());
    
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