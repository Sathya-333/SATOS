BITS 64

section .text

global context_switch

; void context_switch(uint64_t *old_rsp, uint64_t new_rsp)
;
; RDI = address where the current RSP should be saved
; RSI = new thread's RSP

context_switch:
    ; Save the current stack pointer.
    mov [rdi], rsp

    ; Switch to the new thread's stack.
    mov rsp, rsi

    ret