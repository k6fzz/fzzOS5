#include "idt.h"

#include <stddef.h>
#include <stdint.h>

#include "../util/printf.h"
#include "../device/serial/serial.h"
#include "../device/PIC/pic.h"
#include "../memory/memory.h"
#include "../io/io.h"
#include "../device/keyboard/keyboard.h"


static const char* _exception_messages[32] =
{
    "Division by Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Detected Overflow",
    "Out of Bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double Fault",
    "Coprossor Segment Overrun",
    "Bad TSS",
    "Segment not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown INT",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

struct idt_entry
{
    uint16_t base15_0;
    uint16_t kernel_cs;
    uint8_t ist;
    uint8_t attributes;
    uint16_t base31_16;
    uint32_t base63_32;
    uint32_t reserved;
}__attribute__((packed));

__attribute__((aligned(0x10)))
static struct idt_entry idt[256];
static struct idt_ptr idtr;

extern void* isr_stub_table[];
extern void idt_load(struct idt_ptr* idtr);
extern uint64_t read_cr2();

uint64_t pit_ticks = 0;

void create_descriptor(uint16_t index, void* isr, uint8_t ist, uint8_t cs, uint8_t attr)
{
    struct idt_entry* entry = &idt[index];

    entry->base15_0 = (uint64_t)isr & 0xFFFF;
    entry->base31_16 = ((uint64_t)isr >> 16) & 0xFFFF;
    entry->base63_32 = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    entry->ist = ist;
    entry->kernel_cs = cs;
    entry->attributes = attr;
    entry->reserved = 0;
}

void idt_init()
{
    //uint64_t idt_ptr = hh_code_to_phys((uint64_t)&idt[0]);
    idtr.base = (uint64_t)&idt[0];
    idtr.limit = (uint16_t)(sizeof(idt) - 1);

    for(uint8_t index = 0; index < 47; index++)
    {
        create_descriptor(index, isr_stub_table[index], 0x00, 0x08, 0x8E);
    }

    create_descriptor(127, isr_stub_table[48], 0x00, 0x08, 0x8E);
    create_descriptor(128, isr_stub_table[49], 0x00, 0x08, 0x8E);

    pic_remap();
    pic_disable();

    

    idt_load(&idtr);

    pit_init(10000);

    pic_clear_mask(1);
    pic_clear_mask(0);


    //__asm__ volatile ("lidt %0" : : "m"(idtr));
    //__asm__ volatile ("sti");
}

uint64_t interrupt_handler(uint64_t rsp)
{
    struct interrupt_stack_frame* frame = (struct interrupt_stack_frame*)rsp;
    
    if(frame->int_no < 32)
    {
        if(frame->cs == 0x08)
        {
            //kernel panic
            uint64_t cr2 = read_cr2();
            serial_write(0x3F8, 'e');
            printf("\nCPU Exception! INT %d   ERR %x  CR2 %p\n", frame->int_no, frame->err_no, cr2);
            printf(_exception_messages[frame->int_no]);
            printf("\n");
            printf("SS: %x  RSP: %x  RFLAGS: %x  CS: %x  RIP: %x\n", frame->ss, frame->rsp, frame->rflags, frame->cs, frame->rip);
            printf("rax: %x  rdi: %x", frame->rax, frame->rdi);
            asm("cli");
            asm("hlt");
            for(;;);
        }
    }
    else if (frame->int_no < 48)
    {
        uint64_t irq = frame->int_no - 32;
        if(irq > 1)
        {
            printf("INT: %d  IRQ: %d  \n", frame->int_no, irq);
        }
        switch(irq)
        {
            case 0:
                pit_ticks++;
                break;
            case 1:
                kbd_handle();
                break;
        }

        //serial_write(0x3F8, 'i');
        pic_EOI(irq);
        //printf("EOI\n");
        return rsp;
    }
    else if (frame->int_no == 127)
    {
        printf("Syscall 127\n");
        return rsp;
    }
    return rsp;
}