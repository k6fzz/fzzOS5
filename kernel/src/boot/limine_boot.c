#include <stddef.h>
#include <stdint.h>

#include <limine.h>

#include <bootinfo.h>

#define SECTION_LIMINE __attribute__((section(".limine_reqs"), used))

extern __attribute__((noreturn)) void kernel();
void _limine_start();

static volatile struct limine_bootloader_info_request bootloader_info =
{
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};

static volatile struct limine_stack_size_request stack_request =
{
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0,
    .stack_size = 4096,
};

static volatile struct limine_hhdm_request hhdm_request =
{
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
};

static volatile struct limine_framebuffer_request fb_request =
{
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0,   
};

static volatile struct limine_memmap_request mmap_request =
{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

static volatile struct limine_kernel_address_request kernel_addr_request =
{
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

static volatile struct limine_entry_point_request entry_point_request =
{
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0,
    .entry = &_limine_start
};

SECTION_LIMINE static uint64_t reqs[10] = 
{
    (uint64_t)&bootloader_info,
    (uint64_t)&stack_request,
    (uint64_t)&hhdm_request,
    (uint64_t)&fb_request,
    (uint64_t)&mmap_request,
    (uint64_t)&kernel_addr_request,
    (uint64_t)&entry_point_request,
    NULL,
};

void _limine_start()
{

    boot_info.protocol = LIMINE;
    boot_info.pmm_info


    for(;;)
    {
        asm("hlt");
    }
}