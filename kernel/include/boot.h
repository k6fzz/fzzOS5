#pragma once

#include <stdint.h>
#include <stivale2.h>
#include <limine.h>

//typedef void (*stivale_term_write)(const char *string, uint64_t length);
//typedef void (*limine_term_write)(struct limine_terminal* terminal, const char* string, uint64_t length);
//void* stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id);

//void kinit_stivale2(struct stivale2_struct* stivale2_struct);

enum bootloader_type
{
    STIVALE2,
    LIMINE,
};

struct bootinfo_t
{
    enum bootloader_type type;
    struct stivale2_struct_tag_kernel_base_address* tag_kernel_base_address;
    struct stivale2_struct_tag_cmdline* tag_cmdline;
    struct stivale2_struct_tag_memmap* tag_memmap;
    struct stivale2_struct_tag_framebuffer* tag_framebuffer;
    struct stivale2_struct_tag_modules* tag_modules;
    struct stivale2_struct_tag_rsdp* tag_rsdp;
    struct stivale2_struct_kernel_file_v2* tag_kernel_file;
    struct stivale2_struct_tag_hhdm* tag_hhdm;
    struct stivale2_struct_tag_pmrs* tag_pmrs;
    struct limine_terminal* limine_terminal;
};

extern struct bootinfo_t boot_info;