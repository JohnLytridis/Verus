#include "pic.h"
#include "io.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

void pic_remap(void) {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20);  // IRQ0-7 -> interrupts 32-39
    outb(PIC2_DATA, 0x28);  // IRQ8-15 -> interrupts 40-47

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0xFD); // binary 11111101 — only IRQ1 (keyboard) unmasked
outb(PIC2_DATA, 0xFF); // mask everything on the second PIC
}