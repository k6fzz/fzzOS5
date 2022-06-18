#pragma once

#include <stdint.h>
#include <stddef.h>

enum task_state
{
    task_running,
    task_hold,
    task_paused,
    task_terminate
};

struct thread_control_block
{
    uint64_t kernel_stack_top;
    uint64_t task_cr3;
    struct thread_control_block* next;
    uint64_t state;
};

void multitasking_init();
void switch_to_task(struct thread_control_block* thread);



struct task_registers
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t int_no;
    uint64_t err_no;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((packed));

struct task
{
    struct task_registers* regs;
};