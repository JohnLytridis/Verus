#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "shell.h"
#include "pit.h"
#include "mem.h"

void kmain(unsigned int magic, unsigned int mbi) {
    vga_clear();
    vga_print("Hello, welcome to Verus!\n");

    mem_init(magic, mbi);
    idt_init();
    pic_remap();
    keyboard_init();
    pit_init();
    shell_init();

    asm volatile ("sti");

    while (1) {
        asm volatile ("hlt");
    }
}