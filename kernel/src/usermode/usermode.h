#pragma once

#include <stddef.h>
#include <stdint.h>

#define MSR_EFER 0xC0000080
#define MSR_STAR 0xC0000081



void user_init();
void user_enter(void* func, uint8_t* stack);
uint64_t syscall_handler(uint64_t num, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5);

extern uint64_t read_msr(uint64_t MSR);
extern void to_usermode(void* user_function, void* stack);