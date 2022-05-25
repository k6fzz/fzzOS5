#include "cpu.h"

void cpu_init()
{
    uint64_t a,b,c,d;
    __cpuid(0,a,b,c,d);
}