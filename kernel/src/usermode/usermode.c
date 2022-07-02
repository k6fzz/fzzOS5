#include "usermode.h"
#include "../memory/memory.h"
#include <kprintf.h>


extern void enable_sce();
extern void __syscall_handler();
extern void enable_syscall(uint64_t STAR, void* handler);

void user_init()
{
    enable_syscall(0x00180008, &__syscall_handler);
    
    //enable_sce();
    //set_lstar(__syscall_handler);
}

uint64_t syscall_handler(uint64_t num, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5)
{
    switch(num)
    {
        case 0:         //read
            break;
        case 1:         //write
            printf((char*)param1);
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


