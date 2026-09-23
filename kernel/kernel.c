#include <stdint.h>

#include "cpu/idt.h"
#include "cpu/pic.h"
#include "cpu/timer.h"
#include "sched.h"

#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static uint8_t row = 0;
static uint8_t column = 0;

static int thread_1;
static int thread_2;

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

static void print_number(uint64_t number)
{
    char buffer[21];
    int i = 0;

    if (number == 0)
    {
        print("0");
        return;
    }

    while (number > 0)
    {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0)
    {
        char digit[2];

        digit[0] = buffer[--i];
        digit[1] = '\0';

        print(digit);
    }
}

/* ------------------------------------------------------------ */
/* Thread 1                                                     */
/* ------------------------------------------------------------ */

static void test_thread_1(void)
{
    uint64_t last_tick = 0;

    print("\nTHREAD 1: ONLINE");

    while (1)
    {
        uint64_t ticks = timer_get_ticks();

        if (ticks >= last_tick + 50)
        {
            last_tick = ticks;
            print("\n[T1 RUNNING]");
        }

        __asm__ volatile ("nop");
    }
}

/* ------------------------------------------------------------ */
/* Thread 2                                                     */
/* ------------------------------------------------------------ */

static void test_thread_2(void)
{
    uint64_t last_tick = 0;

    print("\nTHREAD 2: ONLINE");

    while (1)
    {
        uint64_t ticks = timer_get_ticks();

        if (ticks >= last_tick + 50)
        {
            last_tick = ticks;
            print("\n[T2 RUNNING]");
        }

        __asm__ volatile ("nop");
    }
}

/* ------------------------------------------------------------ */
/* Kernel entry                                                  */
/* ------------------------------------------------------------ */

void kmain(void)
{
    clear_screen();

    print("========================================\n");
    print("          SATOS KERNEL v0.1\n");
    print("========================================\n\n");

    print("Architecture : x86_64\n");
    print("Boot status  : OK\n");
    print("Kernel       : ONLINE\n\n");

    idt_init();
    pic_init();
    timer_init();
    scheduler_init();

    thread_1 = thread_create(test_thread_1);
    thread_2 = thread_create(test_thread_2);

    if (thread_1 >= 0 && thread_2 >= 0)
    {
        print("Threads created\n");
    }

    print("Interrupts   : READY\n");
    print("Timer        : READY\n\n");

    __asm__ volatile ("sti");

    print("BEFORE SWITCH\n");

    scheduler_switch_to(thread_1);

    print("AFTER THREAD 1\n");

    while (1)
    {
        static uint64_t last_display = 0;

        uint64_t current_ticks = timer_get_ticks();

        if (current_ticks >= last_display + 100)
        {
            last_display = current_ticks;

            print("\nTimer ticks: ");
            print_number(current_ticks);
        }

        __asm__ volatile ("hlt");
    }
}