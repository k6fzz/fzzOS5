#pragma once

#include <stdint.h>
#include "stivale2.h"

void (*term_write)(const char *string, uint64_t length);
//void* stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

void kinit_stivale2(struct stivale2_struct* stivale2_struct);


struct bootinfo_t
{
    struct stivale2_struct_tag_kernel_base_address* tag_kernel_base_address;
    struct stivale2_struct_tag_cmdline* tag_cmdline;
    struct stivale2_struct_tag_memmap* tag_memmap;
    struct stivale2_struct_tag_framebuffer* tag_framebuffer;
    struct stivale2_struct_tag_modules* tag_modules;
    struct stivale2_struct_tag_rsdp* tag_rsdp;
    struct stivale2_struct_kernel_file_v2* tag_kernel_file;
};

extern struct bootinfo_t boot_info;