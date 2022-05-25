bits 64


; gdtptr*, data, code
global loadGDT:function
loadGDT:
    cli
    lgdt [rdi]          ;load GDT

    mov ax, 0x38        ;TSS segment
    ltr ax              ;load TSS
    mov ax, 0x30        ;kernel data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi            ;Pop the return address
    mov rax, 0x28       ;kernel code
    push rax           ;Push the segment
    push rdi           ;Push the return address
    retfq