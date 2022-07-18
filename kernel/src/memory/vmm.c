#include "vmm.h"
#include <pmm.h>
#include "memory.h"
#include <libk/kstring.h>
#include <kprintf.h>
#include <boot.h>
#include <stivale2.h>
#include <debug.h>
#include <framebuffer.h>
#include <serial.h>

#define VMM_START_OF_FB     0xC0000000
#define VMM_SIZE_OF_FB      0x7E9000

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

extern const struct pmm_info pmm_info;

extern void vmm_flush_tlb(void* vaddr);
extern uint64_t vmm_read_cr3();
extern void vmm_write_cr3(uint64_t value);
extern uint32_t vmm_read_efer();
extern void vmm_enable_nxe();


struct PageTable* RootPageDirectory = {0};

struct PageTable* kernel_cr3 = {0};

uint64_t magic = 0xdeadbeef;

struct PageTable* vmm_create_page_table()
{
    void* page = pmm_allocpage();   //get a page
    if(page == NULL) return NULL;   //If it's null, we can't continue

    memset((uint8_t*)page, 0, 4096);    //clear the page
    return (struct PageTable*)phys_to_hh_data((uint64_t)page);
    
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
        if(!newentry) printf("Null page allocated!\n");
        pagemap->entry[index] = newentry | flags;
        //printf("pagemap %p  ", pagemap->entry[index]);
        return (struct PageTable*)(pagemap->entry[index] & (~0x1ff));
    }
}

void vmm_map_2Mpage(struct PageTable* pagetable, uint64_t virtual, uint64_t physical, uint64_t flags)
{
    uint64_t vaddr = virtual;
    uint64_t index2, index3, index4;
    vaddr >>= 12;
    //index1 = vaddr & 0x1ff;
    vaddr >>= 9;
    index2 = vaddr & 0x1ff;
    vaddr >>= 9;
    index3 = vaddr & 0x1ff;
    vaddr >>= 9;
    index4 = vaddr & 0x1ff;

    struct PageTable* PML4;
    struct PageTable* PML3;
    struct PageTable* PML2; 

    PML4 = pagetable;
    PML3 = vmm_get_pagemap(PML4, index4, flags);
    PML2 = vmm_get_pagemap(PML3, index3, flags);

    PML2->entry[index2] = physical | flags | PTE_PAGESIZE;
}

void vmm_map_4Kpage(struct PageTable* pagetable, uint64_t virtual, uint64_t physical, uint64_t flags)
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

    //serial_printf(SERIAL_PORT1, "%p, %p, %p, %p\n", PML4->entry[index4], PML3->entry[index3], PML2->entry[index2], PML1->entry[index1]);
    //serial_printf(SERIAL_PORT1, "%p, %d, %d, %d, %d, %p\n", virtual, index4, index3, index2, index1, physical);

    //vmm_flush_tlb((void*)virtual);

}

void vmm_unmap_page(struct PageTable* pagetable, uint64_t virtual)
{
    struct PageTable* PML4 = (struct PageTable*)(phys_to_hh_data((uint64_t)pagetable));
    struct PageTable* PML3 = NULL;
    struct PageTable* PML2 = NULL;
    struct PageTable* PML1 = NULL;

    uint64_t index1, index2, index3, index4;

    virtual >>= 12;
    index1 = virtual & 0x1ff;
    virtual >>= 9;
    index2 = virtual & 0x1ff;
    virtual >>= 9;
    index3 = virtual & 0x1ff;
    virtual >>= 9;
    index4 = virtual & 0x1ff;

    if(PML4->entry[index4] & 1)
    {
        PML3 = (struct PageTable*)((PML4->entry[index4] >> 12) * 4096);
        if(PML3->entry[index3] & 1)
        {
            PML2 = (struct PageTable*)((PML3->entry[index3] >> 12) * 4096);
            if(PML2->entry[index2] & 1)
            {
                PML1 = (struct PageTable*)((PML2->entry[index2] >> 12) * 4096);
                if(PML1->entry[index1] & 1)
                {
                    PML1->entry[index1] = 0x00;
                }
            }
        }
    }
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

void vmm_PMLwalk(struct PageTable* pagetable)
{
    struct PageTable* PML4 = pagetable;
    struct PageTable* PML3 = NULL;
    struct PageTable* PML2 = NULL;
    struct PageTable* PML1 = NULL;

    serial_printf(SERIAL_PORT1, "Begin Pagewalk\r\nPML 4 %p\r\n", (uint64_t)PML4);
    for(uint64_t i = 0; i < 512; i++)
    {
        if(pagetable->entry[i] & 0x01)
            {
                serial_printf(SERIAL_PORT1, "4,%p,%d,%p\r\n",pagetable, i, pagetable->entry[i]);
                PML3 = (struct PageTable*)(pagetable->entry[i] & ~(0x1FF));
                for(uint64_t j = 0; j < 512; j++)
                {
                    if(PML3->entry[j] & 0x01)
                    {
                        serial_printf(SERIAL_PORT1, "3,%p,%d,%p\r\n",PML3, j, PML3->entry[j]);
                        PML2 = (struct PageTable*)(PML3->entry[j] & ~(0x1FF));
                        for(uint64_t k = 0; k < 512; k++)
                        {
                            if(PML2->entry[k] & 0x01)
                            {
                                serial_printf(SERIAL_PORT1, "2,%p,%d,%p\r\n",PML2, k, PML2->entry[k]);
                                PML1 = (struct PageTable*)(PML3->entry[k] & ~(0x1FF));
                                for(uint64_t l = 0; l < 512; l++)
                                {
                                    if(PML1->entry[l] & 0x01)
                                    {
                                        serial_printf(SERIAL_PORT1, "1,%p,%d,%p\r\n", PML1, l, PML1->entry[l]);
                                    }
                                }//PML1
                            }
                        }//PML2
                    }
                }//PML3
            }
    }//PML4

    serial_printf(SERIAL_PORT1, "End Pagewalk\r\n");
}

void vmm_init()
{
    kernel_cr3 = (struct PageTable*)vmm_read_cr3();
    printf("CR3: %p\n", (uint64_t)kernel_cr3);
    uint32_t efer = vmm_read_efer();
    uint32_t NXE = efer & (1 << 11);
    printf("EFER: %p  NXE: %d\n", efer, NXE);
 
    //Create the Kernel PML4 table
    RootPageDirectory = vmm_create_page_table();

    //Figure out where the kernel is and how big it is from the boot_info struct
    uint64_t kernel_physical = boot_info.tag_kernel_base_address->physical_base_address;
    uint64_t kernel_virtual = boot_info.tag_kernel_base_address->virtual_base_address;
    uint64_t kernel_offset = kernel_virtual - kernel_physical;

/*PMRS Permissions -
    0 - ---
    1 - --E
    2 - -W-
    3 - -WE
    4 - R--
    5 - R-E
    6 - RWE
*/

/*PTE Permissions
    PTE_PRESENT             Present if Set
    PTE_READWRITE           Writable if set
    PTE_USER_SUPERVISOR     User accessable if set
    PTE_EXECUTE_DISABLE     Disables execute if set (if EFER:11 is set)
*/

    for(uint64_t i = 0; i < boot_info.tag_pmrs->entries; i++)
    {
        uint64_t virt = boot_info.tag_pmrs->pmrs[i].base;
        uint64_t phys = virt - kernel_offset;
        uint64_t flags = 0;
        uint64_t perms = boot_info.tag_pmrs->pmrs[i].permissions;
        
        if(perms & 0x02) //is read/write bit set?
        {
            flags |= PTE_READWRITE;
        }

        flags |= PTE_PRESENT;
        
        //DEBUG_MSG("Base: %p  Length: %x  Flags: %x\n", boot_info.tag_pmrs->pmrs[i].base, boot_info.tag_pmrs->pmrs[i].length, boot_info.tag_pmrs->pmrs[i].permissions);
        for(uint64_t j = 0; j < boot_info.tag_pmrs->pmrs[i].length; j+=0x1000)
        {
            vmm_map_4Kpage(RootPageDirectory, virt + j, phys + j, flags);
            //DEBUG_MSG("Kernel : %p %p %x\n", virt + j, phys + j, flags); 
        } 
    }


    //uint64_t kernel_phys = boot_info.tag_kernel_base_address->physical_base_address;
    //uint64_t kernel_virt = boot_info.tag_kernel_base_address->virtual_base_address;
    //uint64_t kernel_size = (uint64_t)&_end_of_kernel - (uint64_t)&_start_of_kernel;

    //printf("Kernel Virtual: %p   Kernal Physical: %p\n", kernel_virt, kernel_phys);

    //printf("Kernel: %p - %p Size: %d\n", &_start_of_kernel, &_end_of_kernel, kernel_size);
    //printf("Text:   %p - %p \n", &_start_of_text, &_end_of_text);
    //printf("Data:   %p - %p \n", &_start_of_data, &_end_of_data);
    //printf("ROData: %p - %p \n", &_start_of_rodata, &_end_of_rodata);
    //printf("BSS:    %p - %p \n", &_start_of_bss, &_end_of_bss);

    //vmm_pagewalk((uint64_t)&_start_of_kernel, (uint64_t*)read_cr3());
    //vmm_pagewalk((uint64_t)&_end_of_kernel, (uint64_t*)read_cr3());

    

    //printf("Root = %p\n", RootPageDirectory);

    //RootPageDirectory->entry[256] = kernel_cr3->entry[256];
    //RootPageDirectory->entry[511] = kernel_cr3->entry[511];

    //vmm_map_4Kpage(RootPageDirectory, 0xFFFFFFFFC0001000, (uint64_t)pmm_allocpage(), PTE_PRESENT | PTE_READWRITE | PTE_USER_SUPERVISOR);

    //Map the Kernel to 0xFFFFFFF8000...
    //TODO - Make the pages sensitive to RO/RW
    //for(uint64_t i = 0; i < (kernel_size / 4096 + 1); i++)
    //{
    //    vmm_map_4Kpage(RootPageDirectory, kernel_virt + (0x1000 * i), kernel_phys + (0x1000 * i), PTE_PRESENT | PTE_READWRITE);
    //}

    //Map Physical Memory to 0xFFFF8...
    printf("HHDM: %p\n", boot_info.tag_hhdm->addr);
    for(uint64_t i = 0; i < pmm_info.totalmem ; i+=0x1000)
    {   
        vmm_map_4Kpage(RootPageDirectory, (boot_info.tag_hhdm->addr + i), i, PTE_PRESENT | PTE_READWRITE);
    }

    DEBUG_MSG("New CR3: %p -- & %p \n", (uint64_t)RootPageDirectory, &RootPageDirectory);
    printf("New CR3: %p -- & %p \n", (uint64_t)RootPageDirectory, &RootPageDirectory);

    //Load new CR3
    //vmm_write_cr3((uint64_t)RootPageDirectory);

    //DEBUG_MSG("CR3 Loaded\n");

    //serial_printf(SERIAL_PORT1, "New PML4\r\n");

    //void *a, *b, *c, *d;

    //a=pmm_allocpage();
    //b=pmm_allocpage();
    //c=pmm_allocpage();
    //d=pmm_allocpage();

    //printf("%p, %p, %p, %p", a,b,c,d);

    //vmm_PMLwalk(kernel_cr3);

    //DEBUG_MSG("New PML4\n");
    //vmm_PMLwalk(RootPageDirectory);

    //vmm_map_page(kernel_cr3, 0xFFFFFFFFC0001000, (uint64_t)pmm_allocpage(), PTE_PRESENT | PTE_READWRITE);
    //uint64_t* value = (uint64_t*)0xFFFFFFFFC0001000;
    //*value = 0xdeadbeef;
    //printf("Magic: %p\n", *value);

    //vmm_pagewalk((uint64_t)&_start_of_kernel, (uint64_t*)RootPageDirectory);


    //cycle through page entries


    
}

