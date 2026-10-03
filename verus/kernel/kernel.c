#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"

void kmain(void) {
    vga_print("Hello, welcome to Verus!");

    idt_init();
    pic_remap();
    keyboard_init();

    asm volatile ("sti"); // enable interrupts

    while (1) {
    }
}