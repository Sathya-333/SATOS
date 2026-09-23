BITS 64

section .text

global isr0
global irq0

extern interrupt_handler
extern timer_handler
extern scheduler_tick

; ------------------------------------------------------------
; CPU exception 0
; ------------------------------------------------------------

isr0:
    push 0
    call interrupt_handler
    add rsp, 8
    iretq


; ------------------------------------------------------------
; IRQ0 - PIT timer interrupt
; ------------------------------------------------------------

irq0:
    ; Save all general-purpose registers.
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; R12 has already been saved above.
    ; Use it to remember our original RSP.
    mov r12, rsp

    ; Align RSP to a 16-byte boundary for the C call.
    and rsp, -16

    ; Update the timer tick count.
    call timer_handler

    ; Tell the PIC that IRQ0 has been handled.
    ; This MUST happen before scheduler_tick()
    ; because scheduler_tick() may switch threads.
    mov al, 0x20
    out 0x20, al

    ; Ask the scheduler whether another thread should run.
    call scheduler_tick

    ; Restore the exact stack pointer from before alignment.
    mov rsp, r12

    ; Restore all registers.
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Return from the hardware interrupt.
    iretq