#include "usermode.h"
#include "../memory/memory.h"



extern void enable_sce();



void user_init()
{
    enable_sce();
}




