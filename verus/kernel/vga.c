#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((unsigned short*) 0xB8000)
#define WHITE_ON_BLACK 0x0F

static int cursor_pos = 0;

void vga_print(const char *str) {
    unsigned short *vga = VGA_MEMORY;
    for (int i = 0; str[i] != '\0'; i++) {
        vga[cursor_pos] = (WHITE_ON_BLACK << 8) | str[i];
        cursor_pos++;
    }
}