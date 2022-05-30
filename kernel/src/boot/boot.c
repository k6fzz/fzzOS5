#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"
#include "boot.h"
#include "../util/printf.h"
#include "../kernel.h"

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


//stivale2 Header Structure
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    // Bit 1, if set, causes the bootloader to return to us pointers in the
    // higher half, which we likely want since this is a higher half kernel.
    // Bit 2, if set, tells the bootloader to enable protected memory ranges,
    // that is, to respect the ELF PHDR mandated permissions for the executable's
    // segments.
    // Bit 3, if set, enables fully virtual kernel mappings, which we want as
    // they allow the bootloader to pick whichever *physical* memory address is
    // available to load the kernel, rather than relying on us telling it where
    // to load it.
    // Bit 4 disables a deprecated feature and should always be set.
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
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

    kernel();


    // We should never get here, but just in case...
    for (;;) {
        asm ("hlt");
    }
}