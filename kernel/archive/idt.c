#include "idt.h"
#include <stddef.h>
#include <stdint.h>

#include "../cpu.h"
#include "../memory/memory.h"
#include "../io/io.h"
#include "../device/PIC/pic.h"

#include "../util/printf.h"





extern void isr0();         //Divide by Zero
extern void isr1();         //Debug
extern void isr2();         //NMI
extern void isr3();         //Breakpoint
extern void isr4();         //Overflow
extern void isr5();         //Bound Range Exceeded
extern void isr6();         //Invalid Opcode
extern void isr7();         //Device not Available
extern void isr8();         //Double Fault
extern void isr9();         //Coprocessor Segment Overrun
extern void isr10();        //Invalid TSS
extern void isr11();        //Segment not Present
extern void isr12();        //Stack-Segment Fault
extern void isr13();        //GPF
extern void isr14();        //Page Fault
extern void isr15();        //Reserved
extern void isr16();        //x87 Floating Point
extern void isr17();        //Alignment Check
extern void isr18();        //Machine Check
extern void isr19();        //SIMD Floating Point
extern void isr20();        //Virtualization Exception
extern void isr21();        //Control Protection Exception
extern void isr22();        //Reserved
extern void isr23();        //Reserved
extern void isr24();        //Reserved
extern void isr25();        //Reserved
extern void isr26();        //Reserved
extern void isr27();        //Reserved
extern void isr28();        //Hypervisor Excpetion
extern void isr29();        //VMM Communication Exception
extern void isr30();        //Security Exception
extern void isr31();        //Reserved

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


struct IDT_Descriptor
{
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist;
    uint8_t typeattr;
    uint16_t offset16;
    uint32_t offset32;
    uint32_t zero;
}__attribute__((packed));

struct IDT_Pointer
{
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

static struct IDT_Pointer       idt_ptr;
static struct IDT_Descriptor    idt[256] __attribute__((aligned(8)));



extern uint64_t idt_load(uint64_t idt_ptr);

void idt_create_descriptor(uint64_t number, uint64_t base, uint16_t selector, uint8_t flags, uint8_t ist)
{
    idt[number].offset0 = base & 0xFFFF;
    idt[number].offset16 = (base >> 16) & 0xFFFF;
    idt[number].offset32 = (base >> 32);
    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].ist = ist & 0x7;
    idt[number].typeattr = flags;
}

void idt_print_descriptor(uint16_t index)
{
    uint64_t offset = 0;

    offset += ((uint64_t)idt[index].offset32 << 32);
    offset |= ((uint64_t)idt[index].offset16 << 16);
    offset |= ((uint64_t)idt[index].offset0);
    
    printf("Index: %d  Offset0: %x  Offset16: %x  Offset32: %x\n", index, idt[index].offset0, idt[index].offset16, idt[index].offset32);
    printf("    Selector: %x  Type: %x  Offset:%x\n", idt[index].selector, idt[index].typeattr, offset);
}

void idt_init()
{
    idt_ptr.limit = (sizeof(struct IDT_Descriptor) * 256) - 1;
    idt_ptr.base = (uint64_t)hh_code_to_phys((uint64_t)&idt);

    printf("IDT PTR limit = %d  base = 0x%p\n", idt_ptr.limit, idt_ptr.base);

    //Initialize IDT
    for(uint64_t i = 0; i < 256; i++)
    {
        idt_create_descriptor(i, 0, 0x08, 0x8E, 0);
    }

    idt_create_descriptor(0, (uint64_t)isr0, 0x08, 0x0E, 0);
    idt_create_descriptor(1, (uint64_t)isr1, 0x08, 0x0E, 0);
    idt_create_descriptor(2, (uint64_t)isr2, 0x08, 0x0E, 0);
    idt_create_descriptor(3, (uint64_t)isr3, 0x08, 0x0E, 0);
    idt_create_descriptor(4, (uint64_t)isr4, 0x08, 0x0E, 0);
    idt_create_descriptor(5, (uint64_t)isr5, 0x08, 0x0E, 0);
    idt_create_descriptor(6, (uint64_t)isr6, 0x08, 0x0E, 0);
    idt_create_descriptor(7, (uint64_t)isr7, 0x08, 0x0E, 0);
    idt_create_descriptor(8, (uint64_t)isr8, 0x08, 0x8E, 2);
    idt_create_descriptor(9, (uint64_t)isr9, 0x08, 0x0E, 0);
    idt_create_descriptor(10, (uint64_t)isr10, 0x08, 0x0E, 0);
    idt_create_descriptor(11, (uint64_t)isr11, 0x08, 0x0E, 0);
    idt_create_descriptor(12, (uint64_t)isr12, 0x08, 0x0E, 0);
    idt_create_descriptor(13, (uint64_t)isr13, 0x08, 0x0E, 0);
    idt_create_descriptor(14, (uint64_t)isr14, 0x08, 0x0E, 0);
    idt_create_descriptor(15, (uint64_t)isr15, 0x08, 0x0E, 0);
    idt_create_descriptor(16, (uint64_t)isr16, 0x08, 0x0E, 0);
    idt_create_descriptor(17, (uint64_t)isr17, 0x08, 0x0E, 0);
    idt_create_descriptor(18, (uint64_t)isr18, 0x08, 0x0E, 0);
    idt_create_descriptor(19, (uint64_t)isr19, 0x08, 0x0E, 0);
    idt_create_descriptor(20, (uint64_t)isr20, 0x08, 0x0E, 0);
    idt_create_descriptor(21, (uint64_t)isr21, 0x08, 0x0E, 0);
    idt_create_descriptor(22, (uint64_t)isr22, 0x08, 0x0E, 0);
    idt_create_descriptor(23, (uint64_t)isr23, 0x08, 0x0E, 0);
    idt_create_descriptor(24, (uint64_t)isr24, 0x08, 0x0E, 0);
    idt_create_descriptor(25, (uint64_t)isr25, 0x08, 0x0E, 0);
    idt_create_descriptor(26, (uint64_t)isr26, 0x08, 0x0E, 0);
    idt_create_descriptor(27, (uint64_t)isr27, 0x08, 0x0E, 0);
    idt_create_descriptor(28, (uint64_t)isr28, 0x08, 0x0E, 0);
    idt_create_descriptor(29, (uint64_t)isr29, 0x08, 0x0E, 0);
    idt_create_descriptor(30, (uint64_t)isr30, 0x08, 0x0E, 0);
    idt_create_descriptor(31, (uint64_t)isr31, 0x08, 0x0E, 0);

    //asm volatile ("lidt %0;" ::"m"(idt_ptr));

    if(idt_load((uint64_t)&idt_ptr))
        printf("Loaded!\n");

    pic_remap();

    idt_create_descriptor(32, (uint64_t)irq0, 0x08, 0x00, 0);
    idt_create_descriptor(33, (uint64_t)irq1, 0x08, 0x00, 0);
    idt_create_descriptor(34, (uint64_t)irq2, 0x08, 0x00, 0);
    idt_create_descriptor(35, (uint64_t)irq3, 0x08, 0x00, 0);
    idt_create_descriptor(36, (uint64_t)irq4, 0x08, 0x00, 0);
    idt_create_descriptor(37, (uint64_t)irq5, 0x08, 0x00, 0);
    idt_create_descriptor(38, (uint64_t)irq6, 0x08, 0x00, 0);
    idt_create_descriptor(39, (uint64_t)irq7, 0x08, 0x00, 0);
    idt_create_descriptor(40, (uint64_t)irq8, 0x08, 0x00, 0);
    idt_create_descriptor(41, (uint64_t)irq9, 0x08, 0x00, 0);
    idt_create_descriptor(42, (uint64_t)irq10, 0x08, 0x00, 0);
    idt_create_descriptor(43, (uint64_t)irq11, 0x08, 0x00, 0);
    idt_create_descriptor(44, (uint64_t)irq12, 0x08, 0x00, 0);
    idt_create_descriptor(45, (uint64_t)irq13, 0x08, 0x00, 0);
    idt_create_descriptor(46, (uint64_t)irq14, 0x08, 0x00, 0);
    idt_create_descriptor(47, (uint64_t)irq15, 0x08, 0x00, 0);


    for(int i = 32; i < 48; i++)
    {
        idt_print_descriptor(i);
    }

    printf("IDT_P at: %p\n", &idt_ptr);
    printf("IDT at: %p\n", &idt);




}

void isr_handler(uint64_t int_num, void* regs)
{
    printf("Exception %d", int_num);
    
    for(;;);
}

void irq_handler(uint64_t int_num, void* regs)
{
    printf("Interrupt %d", int_num);
    
    for(;;);
}

void ipi_handler(uint64_t int_num, void* regs)
{
    printf("Interrupt %d", int_num);
    for(;;);
}