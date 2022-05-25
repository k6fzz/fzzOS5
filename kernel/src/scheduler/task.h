#pragma once

#include <stdint.h>
#include <stddef.h>

struct thread_control_block
{
    uint64_t kernel_stack_top;
    uint64_t task_cr3;
    struct thread_control_block* next;
    uint64_t state;
};

void multitasking_init();
void switch_to_task(struct thread_control_block* thread);
