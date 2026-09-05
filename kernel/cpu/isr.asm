BITS 64

section .text

global isr0
global irq0

extern interrupt_handler
extern timer_handler

; CPU exception: Divide Error
isr0:
    push 0
    call interrupt_handler
    add rsp, 8
    iretq


; Hardware timer interrupt: IRQ0 -> IDT vector 32
irq0:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp

    call timer_handler

    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Tell the PIC that IRQ0 has been handled.
    mov al, 0x20
    out 0x20, al

    iretq