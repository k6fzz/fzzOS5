bits 64

extern interrupt_handler
global isr_stub_table:object (isr_stub_table.end - isr_stub_table)
global idt_load:function (idt_load.end - idt_load)
global int_test:function

global read_cr2:function (read_cr2.end - read_cr2)

read_cr2:
    mov rax, cr2
    ret
.end:

%macro _pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro _popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro INT_NAME 1
    dq _interrupt%1
%endmacro

%macro INT_ERR 1
_interrupt%1:
    push qword %1
    jmp _interrupt_common
%endmacro

%macro INT_NOERR 1
_interrupt%1:
    push qword 0
    push qword %1
    jmp _interrupt_common
%endmacro

_interrupt_common:
    cld
    _pushaq
    mov rdi, rsp
    call interrupt_handler
    mov rsp, rax
    _popaq
    add rsp, 16
    iretq
.end:

INT_NOERR 0
INT_NOERR 1
INT_NOERR 2
INT_NOERR 3
INT_NOERR 4
INT_NOERR 5
INT_NOERR 6
INT_NOERR 7
INT_ERR 8
INT_NOERR 9
INT_ERR 10
INT_ERR 11
INT_ERR 12
INT_ERR 13
INT_ERR 14
INT_NOERR 15
INT_NOERR 16
INT_ERR 17
INT_NOERR 18
INT_NOERR 19
INT_NOERR 20
INT_NOERR 21
INT_NOERR 22
INT_NOERR 23
INT_NOERR 24
INT_NOERR 25
INT_NOERR 26
INT_NOERR 27
INT_NOERR 28
INT_NOERR 29
INT_ERR 30
INT_NOERR 31

INT_NOERR 32
INT_NOERR 33
INT_NOERR 34
INT_NOERR 35
INT_NOERR 36
INT_NOERR 37
INT_NOERR 38
INT_NOERR 39
INT_NOERR 40
INT_NOERR 41
INT_NOERR 42
INT_NOERR 43
INT_NOERR 44
INT_NOERR 45
INT_NOERR 46
INT_NOERR 47

INT_NOERR 127
INT_NOERR 128



isr_stub_table:
    INT_NAME 0
    INT_NAME 1
    INT_NAME 2
    INT_NAME 3
    INT_NAME 4
    INT_NAME 5
    INT_NAME 6
    INT_NAME 7
    INT_NAME 8
    INT_NAME 9
    INT_NAME 10
    INT_NAME 11
    INT_NAME 12
    INT_NAME 13
    INT_NAME 14
    INT_NAME 15
    INT_NAME 16
    INT_NAME 17
    INT_NAME 18
    INT_NAME 19
    INT_NAME 20
    INT_NAME 21
    INT_NAME 22
    INT_NAME 23
    INT_NAME 24
    INT_NAME 25
    INT_NAME 26
    INT_NAME 27
    INT_NAME 28
    INT_NAME 29
    INT_NAME 30
    INT_NAME 31
    INT_NAME 32
    INT_NAME 33
    INT_NAME 34
    INT_NAME 35
    INT_NAME 36
    INT_NAME 37
    INT_NAME 38
    INT_NAME 39
    INT_NAME 40
    INT_NAME 41
    INT_NAME 42
    INT_NAME 43
    INT_NAME 44
    INT_NAME 45
    INT_NAME 46
    INT_NAME 47
    INT_NAME 127
    INT_NAME 128
.end:

idt_load:
    lidt [rdi]
    sti
    ret
.end:

int_test:
    int 127
    ret
.end: