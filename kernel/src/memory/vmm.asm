bits 64

global read_cr3:function (read_cr3.end - read_cr3)
global write_cr3:function (write_cr3.end - write_cr3)


read_cr3:
    mov rax, cr3
    ret
.end:

write_cr3:
    mov cr3, rdi
    ret
.end: