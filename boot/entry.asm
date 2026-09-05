; ============================================================
; SATOS - x86-64 Boot Entry
; ============================================================

BITS 32

section .multiboot
align 8

mb2_header:
    dd 0xE85250D6
    dd 0
    dd mb2_header_end - mb2_header
    dd -(0xE85250D6 + 0 + (mb2_header_end - mb2_header))

    ; End tag
    dw 0
    dw 0
    dd 8

mb2_header_end:


section .text
global _start
extern kmain

_start:
    cli

    ; Set temporary stack
    mov esp, stack_top

    ; --------------------------------------------------------
    ; Build page tables
    ; --------------------------------------------------------

    ; PML4 -> PDPT
    mov eax, pdpt
    or eax, 0x03
    mov [pml4], eax

    ; PDPT -> Page Directory
    mov eax, page_directory
    or eax, 0x03
    mov [pdpt], eax

    ; Identity-map first 1 GiB using 2 MiB pages
    mov ecx, 0
    mov eax, 0x83

.map_pages:
    mov [page_directory + ecx * 8], eax
    add eax, 0x200000
    inc ecx
    cmp ecx, 512
    jne .map_pages

    ; --------------------------------------------------------
    ; Enable PAE
    ; --------------------------------------------------------

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Load PML4
    mov eax, pml4
    mov cr3, eax

    ; --------------------------------------------------------
    ; Enable Long Mode
    ; --------------------------------------------------------

    mov ecx, 0xC0000080
    rdmsr

    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Load 64-bit GDT
    lgdt [gdt64.pointer]

    ; Enter 64-bit mode
    jmp 0x08:long_mode_entry


BITS 64

long_mode_entry:

    ; Reload data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Set 64-bit stack
    mov rsp, stack_top

    ; Call C kernel
    call kmain

.hang:
    cli
    hlt
    jmp .hang


; ============================================================
; GDT
; ============================================================

section .rodata

gdt64:
    dq 0x0000000000000000
    dq 0x00AF9A000000FFFF
    dq 0x00AF92000000FFFF

.pointer:
    dw gdt64.pointer - gdt64 - 1
    dq gdt64


; ============================================================
; Page tables + stack
; ============================================================

section .bss

align 4096

pml4:
    resq 512

align 4096

pdpt:
    resq 512

align 4096

page_directory:
    resq 512

align 16

stack_bottom:
    resb 16384

stack_top: