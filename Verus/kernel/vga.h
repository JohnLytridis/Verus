#ifndef VGA_H
#define VGA_H

void vga_clear(void);
void vga_putchar(char c);
void vga_print(const char *str);
void vga_set_color(unsigned char new_color);

#endif