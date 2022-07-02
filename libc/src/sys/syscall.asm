bits 64

global _syscall:function (__syscall.end - __syscall)

;uint64_t __syscall(uint64_t num, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5)
;uint64_t __syscall(rdi, rsi, rdx, rcx, r8, r9)
_syscall:
    push rsp        ;push stack pointer
    o64 syscall     
    pop rsp
    ret
.end: