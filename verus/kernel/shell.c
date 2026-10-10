#include "shell.h"
#include "vga.h"
#include "io.h"

#define INPUT_MAX 128
#define PROMPT "verus> "

static char input[INPUT_MAX];
static int input_len = 0;
static int inverted = 0;

static int str_eq(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static int starts_with(const char *s, const char *prefix) {
    while (*prefix) {
        if (*s != *prefix) return 0;
        s++;
        prefix++;
    }
    return 1;
}

static void run_command(void) {
    input[input_len] = '\0';

    if (input_len == 0) {
        return;
    }

    if (str_eq(input, "help")) {
        vga_print("Commands: help, clear, about, echo <text>, invert, version, reboot, halt\n");
    } else if (str_eq(input, "clear")) {
        vga_clear();
    } else if (str_eq(input, "about")) {
        vga_print("Verus - a small kernel for you \n");
    } else if (starts_with(input, "echo ")) {
        vga_print(input + 5);
        vga_putchar('\n');
    } else if (str_eq(input, "invert")) {
        inverted = !inverted;
        vga_set_color(inverted ? 0x70 : 0x0F);
    } else if (str_eq(input, "version")) {
        vga_print("Verus 0.3\n");
    } else if (str_eq(input, "reboot")) {
        outb(0x64, 0xFE);
    } else if (str_eq(input, "halt")) {
        vga_print("System halted.\n");
        while (1) {
            asm volatile ("cli; hlt");
        }
    } else {
        vga_print("Unknown command: ");
        vga_print(input);
        vga_putchar('\n');
    }
}

void shell_init(void) {
    input_len = 0;
    vga_print(PROMPT);
}

void shell_input(char c) {
    if (c == '\n') {
        vga_putchar('\n');
        run_command();
        input_len = 0;
        vga_print(PROMPT);
    } else if (c == '\b') {
        if (input_len > 0) {
            input_len--;
            vga_putchar('\b');
        }
    } else if (input_len < INPUT_MAX - 1) {
        input[input_len++] = c;
        vga_putchar(c);
    }
}