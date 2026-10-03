#include "keyboard.h"
#include "io.h"
#include "vga.h"

static const char scancode_to_ascii[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0,
    0,'q','w','e','r','t','y','u','i','o','p','[',']', 0,
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

void keyboard_init(void) {
    // nothing extra to set up yet — IDT gate registration handles this
}

void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);

    if (scancode < sizeof(scancode_to_ascii) && scancode_to_ascii[scancode]) {
        char str[2] = { scancode_to_ascii[scancode], '\0' };
        vga_print(str);
    }

    outb(0x20, 0x20); // tell PIC the interrupt is handled
}