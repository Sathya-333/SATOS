#include "idt.h"

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_pointer
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_pointer idt_ptr;

extern void isr0(void);
extern void irq0(void);

static void set_idt_entry(int number, void (*handler)(void))
{
    uint64_t address = (uint64_t)handler;

    idt[number].offset_low = address & 0xFFFF;
    idt[number].selector = 0x08;
    idt[number].ist = 0;
    idt[number].type_attr = 0x8E;
    idt[number].offset_middle = (address >> 16) & 0xFFFF;
    idt[number].offset_high = (address >> 32) & 0xFFFFFFFF;
    idt[number].reserved = 0;
}

void interrupt_handler(void)
{
    /* Interrupt 0 reached the kernel. */
}

void idt_init(void)
{
    for (int i = 0; i < 256; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_middle = 0;
        idt[i].offset_high = 0;
        idt[i].reserved = 0;
    }

    set_idt_entry(0, isr0);
    set_idt_entry(32,irq0);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}