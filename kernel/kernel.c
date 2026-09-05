#include <stdint.h>
#include "cpu/idt.h"
#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static uint8_t row = 0;
static uint8_t column = 0;

static void clear_screen(void)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            VGA_MEMORY[y * VGA_WIDTH + x] =
                ((uint16_t)0x07 << 8) | ' ';
        }
    }

    row = 0;
    column = 0;
}

static void print(const char* text)
{
    while (*text)
    {
        if (*text == '\n')
        {
            column = 0;
            row++;
            text++;
            continue;
        }

        VGA_MEMORY[row * VGA_WIDTH + column] =
            ((uint16_t)0x0F << 8) | (uint8_t)*text;

        column++;

        if (column >= VGA_WIDTH)
        {
            column = 0;
            row++;
        }

        text++;
    }
}

void kmain(void)
{
    clear_screen();
    idt_init();

    print("========================================\n");
    print("          SATOS KERNEL v0.1\n");
    print("========================================\n\n");

    print("Architecture : x86_64\n");
    print("Boot status  : OK\n");
    print("Kernel       : ONLINE\n\n");

    print("SATOS> ");

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}