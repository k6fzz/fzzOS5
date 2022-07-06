bits 64

global vmm_read_cr3:function (vmm_read_cr3.end - vmm_read_cr3)
global vmm_write_cr3:function (vmm_write_cr3.end - vmm_write_cr3)
global vmm_flush_tlb:function (vmm_flush_tlb.end - vmm_flush_tlb)
global vmm_read_efer:function (vmm_read_efer.end - vmm_read_efer)
global vmm_enable_nxe:function (vmm_enable_nxe.end - vmm_enable_nxe)

vmm_read_cr3:
    mov rax, cr3
    ret
.end:

vmm_write_cr3:
    mov cr3, rdi
    ret
.end:

vmm_flush_tlb:
    invlpg [rdi]
    ret
.end:

vmm_read_efer:
    mov rcx, 0xC0000080
    rdmsr
    ret
.end:

vmm_enable_nxe:
    mov rcx, 0xC0000080
    rdmsr
    or eax, 0x800
    wrmsr
.end: