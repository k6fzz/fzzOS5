bits 64

global vmm_read_cr3:function (vmm_read_cr3.end - vmm_read_cr3)
global vmm_write_cr3:function (vmm_write_cr3.end - vmm_write_cr3)
global vmm_flush_tlb:function (vmm_flush_tlb.end - vmm_flush_tlb)


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