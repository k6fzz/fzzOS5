#include <stdint.h>
#include <stddef.h>
#include <debug.h>
#include <boot.h>
#include <kprintf.h>
#include "./memory/memory.h"
#include "./interrupts/idt.h"
#include <io.h>
#include <serial.h>
#include "./gdt/gdt.h"
#include <framebuffer.h>
#include "./device/console/console.h"
#include <acpi.h>
#include <libk/liballoc.h>
#include "./device/serial/stty.h"
#include "./usermode/usermode.h"

void kernel()
{
    //printf("Kernel Now\n");
    serial_init(0x3F8);     //COM1
    gdt_init();
    idt_init();
    fb_init();
    console_init();

    pmm_init();
    vmm_init();

    user_init();
    
    printf("Kernel Physical Base Address %x\n", boot_info.tag_kernel_base_address->physical_base_address);
    printf("Kernel Virtual Base Address  %x\n", boot_info.tag_kernel_base_address->virtual_base_address);

    printf("PMRS Entries:  %d\n", boot_info.tag_pmrs->entries);
    //for(uint64_t i = 0; i < boot_info.tag_pmrs->entries; i++)
    //{
    //    serial_printf(SERIAL_PORT1, "Base: %p,  Length: %x,  Permissions: %d\n", boot_info.tag_pmrs->pmrs[i].base, boot_info.tag_pmrs->pmrs[i].length, boot_info.tag_pmrs->pmrs[i].permissions);
    //}
    //printf("--------\n");   
    //printf("Memory Map at %x\n", boot_info.tag_memmap);
    //printf("Framebuffer found at %x\n", boot_info.tag_framebuffer->framebuffer_addr);

    



    printf("Kernel Done!\n"); 

    while(true)
    {
        //printf("while");
    }
}


