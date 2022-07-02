bits 64

global _start:function (_start.end - _start)

section .text
_start:

    o64 syscall     

.loop:
    jmp .loop
.end:


section .data
str:        db "Hello User World!"
str_len:    equ $ - str

