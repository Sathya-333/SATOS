BITS 64

section .text

global context_switch

; void context_switch(uint64_t *old_rsp, uint64_t new_rsp)
;
; RDI = address where current RSP is saved
; RSI = new thread's RSP

context_switch:
    ; Save callee-saved registers.
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; Save the current thread's stack pointer.
    mov [rdi], rsp

    ; Switch to the new thread's stack.
    mov rsp, rsi

    ; Restore the new thread's registers.
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ; Jump to the new thread.
    ret