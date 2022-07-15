#include <stdint.h>
#include <stddef.h>

struct boot_info boot_info;

void _elf_start()
{
    for(;;)
    {
        asm("hlt");
    }
}