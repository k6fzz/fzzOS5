#include <stdint.h>
#include <stddef.h>
#include <boot.h>

struct bootinfo_t boot_info;

void _elf_start()
{
    for(;;)
    {
        asm("hlt");
    }
}