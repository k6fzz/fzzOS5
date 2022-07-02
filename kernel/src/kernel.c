#include <stdint.h>
#include <stddef.h>
//#include "kernel.h"
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

uint64_t user_stack [1024];

void user_function()
{
    
    printf("Hello User World!");
    serial_write_str(0x3F8, "Hello User World!");

    for(;;);
}

void kernel()
{

    //printf("Kernel Now\n");
    serial_init(0x3F8);     //COM1
    gdt_init();
    idt_init();
    fb_init();
    console_init();

    //serial_write(0x3F8, 'm');

    pmm_init();
    vmm_init();

    user_init();
    
    printf("Go to user mode!\n");

    to_usermode(user_function, &user_stack[1023]);

    //serial_write(0x3F8, 'k');
    
    //serial_write_str(SERIAL_PORT1, "Testing!\n\rMore Testing!");

    //serial_printf(SERIAL_PORT1, "Hello World! %p\r\n", boot_info.tag_hhdm->addr);
    //acpi_init();

    //fb_clearscreen(FB_COLOR_BLUE);

    //int_test();

    //stty_sendcmd(0);
    printf("Kernel Done!\n"); 
    putstring("Putstring.\n");

    //putchar('x');

    //serial_write(0x3F8, 'x');


    while(true)
    {
        //printf("while");
    }
}


