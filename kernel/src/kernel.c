#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "./boot/boot.h"
#include "./util/printf.h"
#include "./memory/memory.h"
#include "./interrupts/idt.h"
#include "./io/io.h"
#include "./device/serial/serial.h"
#include "./gdt/gdt.h"
#include "./device/framebuffer/framebuffer.h"
#include "./device/console/console.h"
#include "./device/ACPI/acpi.h"
#include "./memory/liballoc.h"




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

    

    //serial_write(0x3F8, 'k');
    
    //serial_write_str(SERIAL_PORT1, "Testing!\n\rMore Testing!");

    //serial_printf(SERIAL_PORT1, "Hello World! %p\r\n", boot_info.tag_hhdm->addr);
    //acpi_init();

    //fb_clearscreen(FB_COLOR_BLUE);

    //int_test();

 
    printf("Kernel Done!\n");

    //putchar('x');

    //serial_write(0x3F8, 'x');


    while(true)
    {
        //printf("while");
    }
}


