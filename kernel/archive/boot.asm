BITS 32

global entryst2

global GDT64
global GDT64.TSS
global GDT64.TSS.low
global GDT64.TSS.mid
global GDT64.TSS.high
global GDT64.TSS.high32

global GDT64Pointer64

extern kinit_stivale2

KERNEL_VIRTUAL_BASE equ 0xFFFFFFFF80000000

section .rodata
align 16
GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .UserCode: equ $ - GDT64     ; The usermode code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11111010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .UserData: equ $ - GDT64     ; The usermode data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 11110010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .TSS: ;equ $ - GDT64         ; TSS Descriptor
    .len:
    dw 108                       ; TSS Length - the x86_64 TSS is 108 bytes loong
    .low:
    dw 0                         ; Base (low).
    .mid:
    db 0                         ; Base (middle)
    db 10001001b                 ; Flags
    db 00000000b                 ; Flags 2
    .high:
    db 0                         ; Base (high).
    .high32:
    dd 0                         ; High 32 bits
    dd 0                         ; Reserved
GDT64Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.
GDT64Pointer64:                    ; The GDT-pointer.
    dw GDT64Pointer - GDT64 - 1             ; Limit.
    dq GDT64 + KERNEL_VIRTUAL_BASE; Base.

section .stivale2hdr
stivale2hdr:
    dq entryst2 ; Use a different entry point for stivale2
    dq 0 ; Bootloader does not need to set up stack for us
    dq 0x0F
    dq st2fbtag

section .data
st2fbtag: ; Tag asks the bootloader to set up a framebuffer for us
    dq 0x3ecc1bc43d0f7971
    dq st2termtag ; Next tag
    dq 0 ; Width
    dq 0 ; Height
    dq 32 ; BPP
;st2fbwctag: ; Ask the bootloader to set framebuffer as writecombining in MTRRs
;  dq 0x4c7bb07731282e00
;  dq 0 ; No next tag
st2termtag:
    dq 0xa85d499b1823be72
    dq 0 ; Next Tag
    dq 0 ; Flags
    dq 0 ; Callback

extern _bss
extern _bss_end

BITS 64
section .text
  
entryst2:
  cli
  cld

  lgdt [GDT64Pointer64]

  mov rbx, rdi ; Save RDI as it contains bootloader information

  mov rdi, _bss
  mov rcx, _bss_end
  sub rcx, _bss
  xor rax, rax
  rep stosb

  mov rsp, stack_top

  mov rdi, rbx ; Restore RDI

  mov rbp, rsp

  push 0x10
  push rbp
  pushf
  push 0x8
  push .cont
  ;retfq
  iretq ; We need to load cs as 0x8, and segments don't really exist because we are in long mode, so use iretq

.cont:
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov rax, cr0
	and ax, 0xFFFB		; Clear coprocessor emulation
	or ax, 0x2			; Set coprocessor monitoring
	mov cr0, rax

	;Enable SSE
	mov rax, cr4
	or ax, 3 << 9		; Set flags for SSE
	mov cr4, rax

  mov rcx, 0x277 ; PAT Model Specific Register
  rdmsr
  mov rbx, 0xFFFFFFFFFFFFFF
  and rax, rbx
  mov rbx, 0x100000000000000
  or rax, rbx  ; Set PA7 to Write-combining (0x1, WC)
  wrmsr

  xor rbp, rbp
  call kinit_stivale2

  cli
  hlt

section .bss
align 64
stack_bottom:
resb 32768
stack_top: