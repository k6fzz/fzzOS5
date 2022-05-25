#pragma once

#include <cpuid.h>

#include <stddef.h>
#include <stdint.h>

typedef struct cpu_info
{
    uint8_t vendor_id[13];
    uint32_t features;
    uint32_t cache_tlb;
    uint32_t serial;
    

}cpu_info_t;

void cpu_init();

