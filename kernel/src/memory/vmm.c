#include "vmm.h"
#include "pmm.h"
#include "memory.h"
#include "../libk/kstring.h"
#include "../util/printf.h"
#include "../boot/boot.h"
#include "../boot/stivale2.h"

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

extern struct pmm_info pmm_info;

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
    return (struct PageTable*)page;
    
    //return (struct PageTable*)phys_to_hh_data((uint64_t)page);
}

static inline struct PageTable* vmm_get_pagemap(struct PageTable* pagemap, uint64_t index, uint64_t flags)
{
    if(pagemap->entry[index] & 1)
    {
        return (struct PageTable*)(pagemap->entry[index] & ~(0x1ff));
    }
    else
    {
        uint64_t newentry = (uint64_t)vmm_create_page_table();
        pagemap->entry[index] = newentry | flags;
        //printf("create new pagemap %p\n", pagemap->entry[index]);
        return (struct PageTable*)(pagemap->entry[index] & (~0x1ff));
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

    //vmm_flush_tlb((void*)virtual);

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
    kernel_cr3 = (struct PageTable*)read_cr3();
    printf("CR3: %p\n", (uint64_t)kernel_cr3);
 
    uint64_t kernel_phys = boot_info.tag_kernel_base_address->physical_base_address;
    uint64_t kernel_virt = boot_info.tag_kernel_base_address->virtual_base_address;
    uint64_t kernel_size = (uint64_t)&_end_of_kernel - (uint64_t)&_start_of_kernel;

    printf("Kernel Virtual: %p   Kernal Physical: %p\n", kernel_virt, kernel_phys);

    printf("Kernel: %p - %p Size: %d\n", &_start_of_kernel, &_end_of_kernel, kernel_size);
    printf("Text:   %p - %p \n", &_start_of_text, &_end_of_text);
    printf("Data:   %p - %p \n", &_start_of_data, &_end_of_data);
    printf("ROData: %p - %p \n", &_start_of_rodata, &_end_of_rodata);
    printf("BSS:    %p - %p \n", &_start_of_bss, &_end_of_bss);

    //vmm_pagewalk((uint64_t)&_start_of_kernel, (uint64_t*)read_cr3());
    //vmm_pagewalk((uint64_t)&_end_of_kernel, (uint64_t*)read_cr3());

    //Create the Kernel PML4 table
    RootPageDirectory = vmm_create_page_table();

    printf("Root = %p\n", RootPageDirectory);

    //Map the Kernel
    //for(uint64_t i = 0; i < kernel_size; i += 4096)
    //{
    //    vmm_map_page(RootPageDirectory, kernel_virt, kernel_phys, PTE_PRESENT | PTE_READWRITE);
    //}

    //Map Physical Me mory
    //for(uint64_t i = 0; i < pmm_info.totalmem; i += 4096)
    //{
    //    vmm_map_page(RootPageDirectory, 0xFFFF00000000 + i, i, PTE_PRESENT | PTE_READWRITE);
    //}

    vmm_map_page(kernel_cr3, 0xFFFFFFFFC0001000, (uint64_t)pmm_allocpage(), PTE_PRESENT | PTE_READWRITE);

    uint64_t* value = (uint64_t*)0xFFFFFFFFC0001000;

    *value = 0xdeadbeef;

    printf("Magic: %p\n", *value);

    //vmm_pagewalk((uint64_t)&_start_of_kernel, (uint64_t*)RootPageDirectory);
    
}

