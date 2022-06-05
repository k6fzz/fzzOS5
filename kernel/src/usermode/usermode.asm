bits 64

global to_usermode:function (to_usermode.end - to_usermode)
global enable_sce:function (enable_sce.end - enable_sce)
global read_msr:function (read_msr.end - read_msr)

to_usermode:
    mov rcx, rdi
    mov rsp, rsi
    mov r11, 0x0202
    sysret
.end:

read_msr:
    mov rcx, rdi
    rdmsr
    ret
.end:

enable_sce:
    mov rcx, 0xC0000080
    rdmsr
    or eax, 1
    wrmsr
    mov rcx, 0xC0000081
    rdmsr
    mov edx, 0x08200008
    wrmsr
    ret
.end: