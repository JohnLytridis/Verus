#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "shell.h"

void kmain(void) {
    vga_clear();
    vga_print("Hello, welcome to Verus!\nType 'help' for available commands.\n");

    idt_init();
    pic_remap();
    keyboard_init();
    shell_init();

    asm volatile ("sti");
    while (1) {
        asm volatile ("hlt");
    }
}
