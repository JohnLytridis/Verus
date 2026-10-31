#include "keyboard.h"
#include "io.h"
#include "vga.h"
#include "shell.h"

/* Scancode set 1, US QWERTY. Index = scancode (0x00 - 0x39). */
static const char keymap[58] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0, 0,
    'q','w','e','r','t','y','u','i','o','p','[',']', 0, 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

/* Same keys with Shift held. */
static const char keymap_shift[58] = {
    0, 0, '!','@','#','$','%','^','&','*','(',')','_','+', 0, 0,
    'Q','W','E','R','T','Y','U','I','O','P','{','}', 0, 0,
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0, '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' '
};

static int extended = 0;
static int shift_down = 0;
static int caps_lock = 0;

void keyboard_init(void) {
    // nothing extra to set up yet
}

static char translate(unsigned char scancode) {
    char base = keymap[scancode];
    if (base == 0) {
        return 0;
    }
    if (base >= 'a' && base <= 'z') {
        /* letters: Shift and Caps Lock cancel each other out */
        return (shift_down ^ caps_lock) ? keymap_shift[scancode] : base;
    }
    /* digits and symbols: only Shift matters */
    return shift_down ? keymap_shift[scancode] : base;
}

void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);

    if (scancode == 0xE0) {
        extended = 1;
    } else if (extended) {
        extended = 0;
        if (scancode == 0x48) {
            shell_history_up();
        } else if (scancode == 0x50) {
            shell_history_down();
        } else if (scancode == 0x1C) {
            shell_input('\n');
        }
    } else if (scancode == 0x2A || scancode == 0x36) {
        shift_down = 1;
    } else if (scancode == 0xAA || scancode == 0xB6) {
        shift_down = 0;
    } else if (scancode == 0x3A) {
        caps_lock = !caps_lock;
    } else if (scancode == 0x0E) {
        shell_input('\b');
    } else if (scancode == 0x1C) {
        shell_input('\n');
    } else if (scancode < sizeof(keymap)) {
        char c = translate(scancode);
        if (c) {
            shell_input(c);
        }
    }

    outb(0x20, 0x20);
}