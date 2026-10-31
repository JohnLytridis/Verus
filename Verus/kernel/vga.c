#include "vga.h"
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((unsigned short*) 0xB8000)
#define WHITE_ON_BLACK 0x0F

static int cursor_pos = 0;
static unsigned char color = WHITE_ON_BLACK;

static void vga_enable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

static void vga_update_cursor(void) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(cursor_pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((cursor_pos >> 8) & 0xFF));
}

void vga_clear(void) {
    unsigned short *vga = VGA_MEMORY;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (color << 8) | ' ';
    }
    cursor_pos = 0;
    vga_enable_cursor();
    vga_update_cursor();
}

void vga_set_color(unsigned char new_color) {
    unsigned short *vga = VGA_MEMORY;
    color = new_color;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (color << 8) | (vga[i] & 0xFF);
    }
}

static void vga_scroll(void) {
    unsigned short *vga = VGA_MEMORY;
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        vga[i] = vga[i + VGA_WIDTH];
    }
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = (color << 8) | ' ';
    }
    cursor_pos = VGA_WIDTH * (VGA_HEIGHT - 1);
}

void vga_putchar(char c) {
    unsigned short *vga = VGA_MEMORY;

    if (c == '\n') {
        cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
    } else if (c == '\b') {
        if (cursor_pos > 0) {
            cursor_pos--;
            vga[cursor_pos] = (color << 8) | ' ';
        }
    } else {
        vga[cursor_pos] = (color << 8) | (unsigned char)c;
        cursor_pos++;
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        vga_scroll();
    }

    vga_update_cursor();
}

void vga_print(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}