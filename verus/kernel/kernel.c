#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"

void kmain(void) {
    vga_clear();
    vga_print("Hello, welcome to Verus!\n");

    idt_init();
    pic_remap();
    keyboard_init();
    shell_init();

    asm volatile ("sti");

    while (1) {
    }
}