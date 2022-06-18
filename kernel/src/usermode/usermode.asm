bits 64

global to_usermode:function (to_usermode.end - to_usermode)
global enable_sce:function (enable_sce.end - enable_sce)
global read_msr:function (read_msr.end - read_msr)
global __syscall:function (__syscall.end - __syscall)
gloval __syscall_handler:function (__syscall_handler.end - __syscall_handler)

;void syscall_handler(num, param1, param2, param3, param4, param5);
extern syscall_handler

; void to_usermode(void* user_func, void* stack);
to_usermode:
    mov rcx, rdi        ;New instruction pointer
    mov rsp, rsi        ;New stack pointer
    mov r11, 0x0202     ;Set eflags
    o64 sysret
.end:

read_msr:
    mov rcx, rdi
    rdmsr
    ret
.end:

;TODO: Make this have the prototype: void enable_sce(uint64_t STAR, uint64_t LSTAR, uint64_t CSTAR);
enable_sce:
    mov rcx, 0xC0000080
    rdmsr
    or eax, 1
    wrmsr
    mov rcx, 0xC0000081
    rdmsr
    mov edx, 0x00200008
    wrmsr
    ret
.end:

;uint64_t __syscall(uint64_t num, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5)
;uint64_t __syscall(rdi, rsi, rdx, rcx, r8, r9)
__syscall:
    push rsp        ;push stack pointer
    o64 syscall     
    pop rsp
    ret
.end:

__syscall_handler:
    push rbx
    push rsp
    push rbp
    push r12
    push r13
    push r14
    push r15
    push rcx
    call syscall_handler
    pop rcx
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rsp
    pop rbx
    o64 sysret
.end: