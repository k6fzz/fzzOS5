#include "gdt.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t rsp2stack[4096];

union gdt_entry
{
    uint64_t raw;
    struct
    {
        uint64_t limit0 : 16;
        uint64_t base0 : 16;
        uint64_t base16 : 8;
        uint64_t accessed : 1;
        uint64_t readwrite : 1;
        uint64_t direction : 1;
        uint64_t executable : 1;
        uint64_t descriptor_type : 1;
        uint64_t DPL : 2;
        uint64_t present : 1;
        uint64_t limit16 : 4;
        uint64_t reserved0 : 1;
        uint64_t longmode : 1;
        uint64_t size : 1;
        uint64_t granularity : 1;
        uint64_t base24 : 8;
    };
}__attribute__((packed));

struct gdt_sys_entry
{
    uint64_t limit0 : 16;
    uint64_t base0 : 16;
    uint64_t base16 : 8;
    uint64_t segment_type : 4;
    uint64_t descriptor_type : 1;
    uint64_t DPL : 2;
    uint64_t present : 1;
    uint64_t limit16 : 4;
    uint64_t reserved0 : 1;
    uint64_t longmode : 1;
    uint64_t size : 1;
    uint64_t granularity : 1;
    uint64_t base24 : 8;
    uint64_t base32 : 32;
    uint64_t reserved1 : 32;
}__attribute__((packed));

struct gdt_table
{
    union gdt_entry null0;          //0x00
    union gdt_entry kcode;          //0x08       
    union gdt_entry kdata;          //0x10
    union gdt_entry null1;          //0x18
    union gdt_entry ucode;          //0x20
    union gdt_entry udata;          //0x28
    union gdt_entry null2;          //0x30
    union gdt_entry null3;          //0x38
    union gdt_entry null4;          //0x40
    union gdt_entry null5;          //0x48
    struct gdt_sys_entry tss_entry; //0x50
}__attribute__((packed));

struct tss_table
{
    uint32_t reserved0;
    uint64_t RSP0;
    uint64_t RSP1;
    uint64_t RSP2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
}__attribute__((packed));

struct gdt_pointer
{
    uint16_t limit;
    uint64_t base;
}__attribute__((packed));

static struct gdt_table gdt;
static struct tss_table tss;
static struct gdt_pointer gdtr;

extern void gdt_load(uint64_t* gdtr, uint8_t tss_segment);
extern uint8_t stack[4096];
uint8_t ist0[4096];
uint8_t ist1[4096];
uint8_t ist2[4096];

void print_gdt()
{

}

void gdt_init()
{
    gdt.null0.raw = 0x00;
    gdt.kcode.raw = 0x00AF9A000000FFFF;
    gdt.kdata.raw = 0x00AF92000000FFFF;
    gdt.null1.raw = 0x00;
    gdt.ucode.raw = 0x00AFFA000000FFFF;
    gdt.udata.raw = 0x00AFF2000000FFFF;
    gdt.null2.raw = 0x00;
    gdt.null3.raw = 0x00;
    gdt.null4.raw = 0x00;
    gdt.null5.raw = 0x00;

    gdt.tss_entry.base0 = (uint64_t)&tss & 0xFFFF;
    gdt.tss_entry.base16 = ((uint64_t)&tss >> 16) & 0xFF;
    gdt.tss_entry.base24 = ((uint64_t)&tss >> 24) & 0xFF;
    gdt.tss_entry.base32 = ((uint64_t)&tss >> 32) & 0xFFFFFFFF;
    gdt.tss_entry.limit0 = sizeof(tss);
    gdt.tss_entry.limit16 = 0;
    gdt.tss_entry.present = 1;
    gdt.tss_entry.DPL = 0;
    gdt.tss_entry.descriptor_type = 0;      
    gdt.tss_entry.segment_type = 0x09;      //64 bit TSS
    gdt.tss_entry.longmode = 0;
    gdt.tss_entry.granularity = 0;
    gdt.tss_entry.reserved0 = 0;
    gdt.tss_entry.reserved1 = 0;

    tss.reserved0 = 0x00;
    tss.RSP0 = (uint64_t)stack + sizeof(stack);
    tss.RSP2 = (uint64_t)rsp2stack + sizeof(rsp2stack);
    tss.ist1 = (uint64_t)ist1 + sizeof(ist1);

    gdtr.base = (uint64_t)&gdt;
    gdtr.limit = sizeof(gdt);

    gdt_load((uint64_t*)&gdtr, 0x50);
}
