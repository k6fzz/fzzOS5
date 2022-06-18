#include "pic.h"
#include <io.h>

#define PIC1		0x20	    // I/O base address for master PIC
#define PIC2		0xA0	    // I/O base address for slave PIC
#define PIC1_CMD	PIC1	    // master PIC command port
#define PIC1_DATA	(PIC1+1)    // master PIC data port
#define PIC2_CMD	PIC2	    // slave PIC command port
#define PIC2_DATA	(PIC2+1)    // slave PIC data port
#define PIC_EOI     0x20

uint64_t ticks;

void pic_EOI(uint8_t IRQ)
{
    if(IRQ >= 8)
    {
        outb(PIC2_CMD, PIC_EOI);
    }
    
    outb(PIC1_CMD, PIC_EOI);
}

void pic_set_mask(uint8_t IRQ)
{
    uint16_t port;
    uint8_t value;

    if(IRQ < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        IRQ -= 8;
    }
    value = inb(port) | (1 << IRQ);
    outb(port, value);
}

void pic_clear_mask(uint8_t IRQ)
{
    uint16_t port;
    uint8_t value;

    if(IRQ < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        IRQ -= 8;
    }
    value = inb(port) & ~(1 << IRQ);
    outb(port, value);
}

void pic_disable()
{
    outb(PIC2_DATA, 0xff);
    outb(PIC1_DATA, 0xff);
}

void pic_remap()
{
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11);
    io_wait();
    outb(PIC2_CMD, 0x11);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x20);
    io_wait();

    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();

    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, mask1);
    io_wait();
    outb(PIC2_DATA, mask2);
    io_wait();
}



void pit_init(uint64_t freq)
{
    uint64_t working_freq = freq;
    if(freq < 10000)
        working_freq = 10000;
    
    if(freq > 1193181)
        working_freq = 1193181;

    uint32_t divisor = 1193180 / working_freq;

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xff);
    outb(0x40, divisor >> 8);
}