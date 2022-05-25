#include "usermode.h"
#include "../memory/memory.h"


extern void to_usermode(void* user_function, void* stack);
extern void enable_sce();

void user_init()
{

}


void user_enter(void* func, uint8_t* stack)
{

}