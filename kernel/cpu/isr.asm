BITS 64

section .text

global isr0

extern interrupt_handler

isr0:
    push 0
    call interrupt_handler
    add rsp, 8
    iretq