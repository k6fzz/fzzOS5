#include "usermode.h"
#include "../memory/memory.h"



extern void enable_sce();

uint64_t _syscall(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
    
}

void user_init()
{
    enable_sce();
}

uint64_t syscall_handler(uint64_t num, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5)
{
    switch(num)
    {
        case 0:         //read
            break;
        case 1:         //write
            kprintf(param1);
            break;
        case 2:         //open
            break;
        case 3:         //close
            break;
        case 0x10:      //ioctl
            break;
        default:
            return -1;
    }

    return 0;
}


