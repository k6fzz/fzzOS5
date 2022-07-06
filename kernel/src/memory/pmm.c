#include <pmm.h>
#include "memory.h"
#include <kprintf.h>
#include <boot.h>
#include <debug.h>
#include <stivale2.h>
//#include "bitmap.h"



struct pmm_info pmm_info;

struct pmm_to_vmm_info pmm_vmm_info;

/*static const char* pmm_mmap_type[10] = 
{
    "UNKNOWN",
    "STIVALE2_MMAP_USABLE",                 //1
    "STIVALE2_MMAP_RESERVED",               //2
    "STIVALE2_MMAP_ACPI_RECLAIMABLE",       //3
    "STIVALE2_MMAP_ACPI_NVS" ,              //4
    "STIVALE2_MMAP_BAD_MEMORY",             //5
    "STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE", //0x1000
    "STIVALE2_MMAP_KERNEL_AND_MODULES",     //0x1001
    "STIVALE2_MMAP_FRAMEBUFFER" ,           //0x1002
};
*/

//Bitmap helpers
static inline void bitmap_set(uint8_t* bitmap, uint64_t bit) 
{
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_unset(uint8_t* bitmap, uint64_t bit)
{
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_get(uint8_t* bitmap, uint64_t bit)
{
    return bitmap[bit/8] & (1 << (bit & 8));
}

//TODO: Clean up end of bitmap testing
static inline uint64_t get_first_unset(uint8_t* bitmap, uint64_t size)
{
    //uint64_t remaining = size % 8;
    
    for(uint64_t i = 0; i < size; i++)
    {
        if(bitmap[i] != 0xff)
            for(uint64_t j = 0; j < 8; j++)
            {
                if(!(bitmap[i] & (1 << j)))
                    return (i * 8) + j;
            }
    }

    return 0;
}

/*static inline char* pmm_get_mmap_type(uint64_t type)
{
    switch(type)
    {
        case 1:
            return &pmm_mmap_type[1];
        case 2:
            return &pmm_mmap_type[2];
        case 3:
            return &pmm_mmap_type[3];
        case 4:
            return &pmm_mmap_type[4];
        case 5:
            return &pmm_mmap_type[5];
        case 0x1000:
            return &pmm_mmap_type[6];
        case 0x1001:
            return &pmm_mmap_type[7];
        case 0x1002:
            return &pmm_mmap_type[8];
        default:
            return &pmm_mmap_type[0];
    }
    return NULL;
}*/

//Initialize the Physical Memory Manager
void pmm_init()
{
    //memory map helper variables
    uint64_t mmap_entries = boot_info.tag_memmap->entries;
    struct stivale2_mmap_entry* current_entry = NULL;
    uint64_t mmap_largest_segment_base = 0;     //Address of the largest segment
    uint64_t mmap_largest_segment_size = 0;     //Size of the largest segment
    //uint8_t* type = NULL;

    //iterate through the memory map, find the largest memory segment, calculate total memory size
    for (uint64_t i = 0; i < mmap_entries; i++)
    {
        current_entry = &boot_info.tag_memmap->memmap[i];
        //type = pmm_get_mmap_type(current_entry->type);
        //printf("Addr: 0x%x  Size: %d  Type: %x\n", current_entry->base, current_entry->length, current_entry->type);

        //Calculate total memory
        pmm_info.totalmem = pmm_info.totalmem + current_entry->length;
        
        //Capture mmap framebuffer info
        if(current_entry->type == 0x1002)
        {
            pmm_vmm_info.fb_base = current_entry->base;
            pmm_vmm_info.fb_size = current_entry->length;
        }
        
        
        //If memory isn't usable, continue
        if(current_entry->type != 1)
        {
            continue;
        }

        //Capture the largest segment size
        if(current_entry->length > mmap_largest_segment_size)
        {
            mmap_largest_segment_base = current_entry->base;
            mmap_largest_segment_size = current_entry->length;
        }
    };

    //Populate the PMM state variables
    pmm_info.totalpages = pmm_info.totalmem / 4096;
    pmm_info.usedpages = pmm_info.totalpages;
    pmm_info.bitmap_size = (pmm_info.totalpages / 8) + 1;
    
    if(pmm_info.bitmap_size > current_entry->length) 
        {
            printf("Bitmap too large");
            for(;;){}   //hang
        }
    
    pmm_info.bitmap = (uint8_t*)phys_to_hh_data(mmap_largest_segment_base); 

    printf("Total Memory: %d\n", (pmm_info.totalmem));
    printf("Total Pages: %d   Used Pages: %d\n", pmm_info.totalpages, pmm_info.usedpages);
    printf("Bitmap Addr: %p  Bitmap Size (bytes): %d\n", pmm_info.bitmap, pmm_info.bitmap_size);

    //Set all bitmap bits to 1
    for (uint64_t i = 0; i < pmm_info.bitmap_size; i++)
        {
        pmm_info.bitmap[i] = 0xff;
        //printf("%x\n", pmm_info.bitmap[i]);
        }
    //printf("\n");
    //printf("Bitmap Set\n");
    

    //printf("Number of MMAP Entries: %d\n", mmap_entries);

    //Iterate through the memory map again, unset bitmap for available memory
    for (uint64_t i = 0; i < mmap_entries; i++)
    {
        current_entry = &boot_info.tag_memmap->memmap[i];

        if(current_entry->type == 1)
        {
            uint64_t pages = current_entry->length / 4096;
            uint64_t align = current_entry->base / 4096;
            //printf("Index: %d  Pages: %d\n", align, pages);

            for(uint64_t j = 0; j < pages; j++)
            {
                //uint64_t byteindex = align / 8;
                //uint64_t bitindex = align % 8;
                //pmm_info.bitmap[byteindex] &= ~(1 << bitindex);
                
                bitmap_unset(pmm_info.bitmap, align);

                align++;                    //increment page index
                pmm_info.usedpages--;       //decrement used page counter
            }
        }
    }

    //Make certain the NULL page is unavailable
    bitmap_set(pmm_info.bitmap, 0);
    pmm_info.usedpages++;

    printf("Available Pages: %d\n", (pmm_info.totalpages - pmm_info.usedpages));
    printf("PMM Initialized\n");

}


void* pmm_allocpage()
{
    if(pmm_info.usedpages >= pmm_info.totalpages)
        return NULL;
    
    uint64_t index = get_first_unset(pmm_info.bitmap, pmm_info.bitmap_size);
    
    bitmap_set(pmm_info.bitmap, index);

    pmm_info.usedpages++;

    DEBUG_MSG("PMM ALLOC: %p\n", (index * PAGE_SIZE));

    return (void*)(index * PAGE_SIZE);
}


void pmm_freepage(void* page)
{
    uint64_t index = (uint64_t)page / PAGE_SIZE;
    bitmap_unset(pmm_info.bitmap, index);
    pmm_info.usedpages--;
}

void* pmm_allocpages(uint64_t count)
{
    if(pmm_info.usedpages >= pmm_info.totalpages)
        return NULL;
    
    uint64_t index = get_first_unset(pmm_info.bitmap, pmm_info.bitmap_size);
    for(uint64_t i = 0; i < count; i++)
    {
        if(bitmap_get(pmm_info.bitmap, i) == 1)
        {
            continue;
        }
        else
        {
            return NULL;
        }
    }

    pmm_info.usedpages += count;

    return (void*)(index * PAGE_SIZE);
}



uint64_t pmm_freepages(void* page, uint64_t count)
{
    uint64_t pg = (uint64_t)page;

    for(uint64_t i=0; i < count; i++)
    {
        pmm_freepage((void*)pg);
        pg += 0x1000;
    }

    return 0;
}

uint64_t pmm_get_free_memory()
{
    return (pmm_info.totalpages - pmm_info.usedpages) * PAGE_SIZE;
}

uint64_t pmm_get_total_memory()
{   
    return (pmm_info.totalpages) * PAGE_SIZE;
}