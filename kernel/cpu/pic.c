#include "pic.h"
#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

void pic_send_eoi(void)
{
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_init(void)
{
    /*
     * Start PIC initialization.
     */
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    /*
     * Remap IRQs:
     *
     * Master PIC: IRQ0-7  -> interrupts 32-39
     * Slave PIC:  IRQ8-15 -> interrupts 40-47
     */
    outb(PIC1_DATA, 32);
    outb(PIC2_DATA, 40);

    /*
     * Tell the PICs how they are connected.
     */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    /*
     * Use 8086/88 mode.
     */
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /*
     * Mask all hardware interrupts for now.
     *
     * We will enable IRQ0 after the PIT is ready.
     */
    outb(PIC1_DATA, 0xFE);
    outb(PIC2_DATA, 0xFF);
}