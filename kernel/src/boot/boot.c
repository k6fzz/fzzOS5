#include <stdint.h>
#include <stddef.h>
#include <boot.h>
#include <kprintf.h>

extern void kernel();

uint8_t stack[4096];

struct bootinfo_t boot_info;

//Request a terminal
static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

//Request Framebuffer
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

enum stivale_header_flags
{
    stivale_header_higherhalf               = (1 << 1),     //return pointers in the higher half
    stivale_header_PMR                      = (1 << 2),     //enable Protected Memory Ranges (respect ELF PHDR)
    stivale_header_virtual_kernel_mapping   = (1 << 3),     //fully virtual kernel mappings
    stivale_header_alwaysset                = (1 << 4)      //always set, disables deprecated feature
};

//stivale2 Header Structure
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    //.flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .flags = stivale_header_higherhalf | stivale_header_PMR | stivale_header_virtual_kernel_mapping | stivale_header_alwaysset,
    .tags = (uintptr_t)&framebuffer_hdr_tag
};



static void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }
        if (current_tag->identifier == id) {
            return current_tag;
        }
        current_tag = (void *)current_tag->next;
    }
}

// Entry point. We're going to make sure the bootloader did what it's supposed to do, then pass off to the kernel.
//void _start(struct stivale2_struct *stivale2_struct) 
void _start(struct stivale2_struct* stivale2_struct)
{



    // Let's get the terminal structure tag from the bootloader.
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);

    // Check if the tag was actually found.
    if (term_str_tag == NULL) {
        // It wasn't found, just hang...
        for (;;) {
            asm ("hlt");
        }
    }

    void* term_write_ptr = (void *)term_str_tag->term_write;
    term_write = term_write_ptr;
    //printf("Terminal Initialized\n");

    //Grab the Framebuffer
    boot_info.tag_framebuffer = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    if (boot_info.tag_framebuffer == NULL)
    {
    //    printf("Framebuffer Not Found\n");
        for( ;; ) 
            asm("hlt");
    }
    //printf("Framebuffer found at %x\n", boot_info.tag_framebuffer->framebuffer_addr);

    //Memory Map
    boot_info.tag_memmap = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
    if (boot_info.tag_memmap == NULL)
    {
    //    printf("Memory Map Not Found\n");
        for( ;; ) 
            asm("hlt");
    }
    //printf("Memory Map at %x\n", boot_info.tag_memmap);

    //Kernel Base Address
    boot_info.tag_kernel_base_address = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
    if (boot_info.tag_kernel_base_address == NULL)
    {
    //    printf("Base Address Not Found\n");
        //for( ;; ) 
        //    asm("hlt");
    }
    //printf("Kernel Physical Base Address %x\n", boot_info.tag_kernel_base_address->physical_base_address);
    //printf("Kernel Virtual Base Address  %x\n", boot_info.tag_kernel_base_address->virtual_base_address);
    //printf("--------\n");

    boot_info.tag_rsdp = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);
    
    boot_info.tag_hhdm = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_HHDM_ID);

    boot_info.tag_modules = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);

    kernel();


    // We should never get here, but just in case...
    for (;;) {
        asm ("hlt");
    }
}