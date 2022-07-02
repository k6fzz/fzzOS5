bits 64

global to_usermode:function (to_usermode.end - to_usermode)
global enable_sce:function (enable_sce.end - enable_sce)
global read_msr:function (read_msr.end - read_msr)
global enable_syscall:function (enable_syscall.end - enable_syscall)
global __syscall_handler:function (__syscall_handler.end - __syscall_handler)

;void syscall_handler(num, param1, param2, param3, param4, param5);
extern syscall_handler

; void to_usermode(void* user_func, void* stack);
to_usermode:
    mov rcx, rdi        ;New instruction pointer
    mov rsp, rsi        ;New stack pointer
    mov r11, 0x3202     ;Set eflags  (IOPL 3, Interrupt Enable, Reserved (1))
    o64 sysret
.end:

read_msr:
    mov rcx, rdi
    rdmsr
    ret
.end:

;void enable_syscall(uint64_t STAR, uint64_t LSTAR);
enable_syscall:
    ;enable SCE
    mov rcx, 0xC0000080
    rdmsr
    or eax, 1
    wrmsr

    ;load STAR
    mov rcx, 0xC0000081
    rdmsr
    mov rdx, rdi
    wrmsr

    ;load LSTAR for SYSCALL handler
    ;EDX:EAX
    ;Load RSI into EDX and EAX. Shift EDX right 32 bits. Write MSR. 
    mov rcx, 0xC0000082
    rdmsr
    mov rax, rsi
    mov rdx, rsi
    shl rdx, 32
    wrmsr

    ret
.end:
    


;TODO: Make this have the prototype: void enable_sce(uint64_t STAR, uint64_t LSTAR);
enable_sce:
    mov rcx, 0xC0000080 ;enable sce
    rdmsr
    or eax, 1
    wrmsr
    mov rcx, 0xC0000081 ;set STAR for SYSRET
    rdmsr
    mov edx, 0x00180008
    ;mov edx, rdi
    wrmsr
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