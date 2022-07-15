#pragma once

#include <stddef.h>
#include <stdint.h>

struct pmm_info;
struct vmm_info;
struct fb_info;

enum bootloader_type
{
    STIVALE2,
    LIMINE,
};

struct boot_info
{
    enum bootloader_type protocol;
    uint64_t memory_map_p;
    uint64_t hhdm_p;
    uint64_t framebuffer_p;
};




extern struct boot_info boot_info;