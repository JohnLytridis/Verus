#include "pit.h"
#include "mem.h"
#include "shell.h"
#include "vga.h"
#include "io.h"

#define INPUT_MAX 128
#define HISTORY_SIZE 16
#define PROMPT "verus> "

static char input[INPUT_MAX];
static int input_len = 0;
static int inverted = 0;

static char history[HISTORY_SIZE][INPUT_MAX];
static int history_count = 0;
static int history_pos = 0;
static char draft[INPUT_MAX];

struct color_entry {
    const char *name;
    unsigned char dark;
    unsigned char bright;
};

static const struct color_entry palette[] = {
    { "white",  0x0, 0xF },
    { "green",  0x2, 0xA },
    { "blue",   0x1, 0x9 },
    { "red",    0x4, 0xC },
    { "yellow", 0x6, 0xE },
    { "cyan",   0x3, 0xB },
};
#define PALETTE_SIZE 6

static int color_index = 0;

static void apply_color(void) {
    if (inverted) {
        vga_set_color((0x7 << 4) | palette[color_index].dark);
    } else {
        vga_set_color(palette[color_index].bright);
    }
}

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

static void str_copy(char *dst, const char *src) {
    while (*src) {
        *dst = *src;
        dst++;
        src++;
    }
    *dst = '\0';
}

static int parse_int(const char **s) {
    int n = 0;
    while (**s == ' ') (*s)++;
    while (**s >= '0' && **s <= '9') {
        n = n * 10 + (**s - '0');
        (*s)++;
    }
    return n;
}

static void print_int(int n) {
    char buf[12];
    int i = 0;
    if (n == 0) { vga_putchar('0'); return; }
    if (n < 0) { vga_putchar('-'); n = -n; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) vga_putchar(buf[--i]);
}

static void print_cpu_vendor(void) {
    unsigned int eax, ebx, ecx, edx;
    char vendor[13];
    asm volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    for (int i = 0; i < 4; i++) {
        vendor[i]     = (ebx >> (8 * i)) & 0xFF;
        vendor[4 + i] = (edx >> (8 * i)) & 0xFF;
        vendor[8 + i] = (ecx >> (8 * i)) & 0xFF;
    }
    vendor[12] = '\0';
    vga_print(vendor);
}

static void history_add(const char *line) {
    if (history_count > 0 && str_eq(history[history_count - 1], line)) {
        return;
    }
    if (history_count == HISTORY_SIZE) {
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            str_copy(history[i], history[i + 1]);
        }
        history_count--;
    }
    str_copy(history[history_count], line);
    history_count++;
}

static void replace_input(const char *text) {
    while (input_len > 0) {
        vga_putchar('\b');
        input_len--;
    }
    while (*text && input_len < INPUT_MAX - 1) {
        input[input_len++] = *text;
        vga_putchar(*text);
        text++;
    }
}

void shell_history_up(void) {
    if (history_count == 0) {
        return;
    }
    if (history_pos == history_count) {
        input[input_len] = '\0';
        str_copy(draft, input);
    }
    if (history_pos > 0) {
        history_pos--;
        replace_input(history[history_pos]);
    }
}

void shell_history_down(void) {
    if (history_pos >= history_count) {
        return;
    }
    history_pos++;
    if (history_pos == history_count) {
        replace_input(draft);
    } else {
        replace_input(history[history_pos]);
    }
}

static void run_command(void) {
    input[input_len] = '\0';

    if (input_len == 0) {
        return;
    }

    if (str_eq(input, "help")) {
        vga_print("Commands: help, clear, about, echo <text>, invert, version,\n");
        vga_print("          reboot, halt, color <name>, banner, cpu, add <a> <b>,\n");
        vga_print("          uptime, meminfo, crash\n");
    } else if (str_eq(input, "clear")) {
        vga_clear();
    } else if (str_eq(input, "about")) {
        vga_print("Verus -  Best kernel out there \n");
    } else if (starts_with(input, "echo ")) {
        vga_print(input + 5);
        vga_putchar('\n');
    } else if (str_eq(input, "invert")) {
        inverted = !inverted;
        apply_color();
    } else if (starts_with(input, "color ")) {
        const char *name = input + 6;
        int found = 0;
        for (int i = 0; i < PALETTE_SIZE; i++) {
            if (str_eq(name, palette[i].name)) {
                color_index = i;
                found = 1;
                break;
            }
        }
        if (found) {
            apply_color();
        } else {
            vga_print("Colors: white, green, blue, red, yellow, cyan\n");
        }
    } else if (str_eq(input, "banner")) {
        vga_print("==========================\n");
        vga_print("        V E R U S         \n");
        vga_print("==========================\n");
    } else if (str_eq(input, "cpu")) {
        vga_print("CPU vendor: ");
        print_cpu_vendor();
        vga_putchar('\n');
    } else if (starts_with(input, "add ")) {
        const char *p = input + 4;
        int a = parse_int(&p);
        int b = parse_int(&p);
        print_int(a + b);
        vga_putchar('\n');
    } else if (str_eq(input, "version")) {
        vga_print("Verus 0.2.5\n");
    } else if (str_eq(input, "reboot")) {
        outb(0x64, 0xFE);
    } else if (str_eq(input, "halt")) {
        vga_print("System halted.\n");
        while (1) {
            asm volatile ("cli; hlt");
        }
        } else if (str_eq(input, "uptime")) {
        unsigned int s = pit_seconds();
        vga_print("Uptime: ");
        print_int(s / 3600);
        vga_print("h ");
        print_int((s % 3600) / 60);
        vga_print("m ");
        print_int(s % 60);
        vga_print("s\n");
    } else if (str_eq(input, "meminfo")) {
        if (!mem_valid()) {
            vga_print("Memory info not available.\n");
        } else {
            vga_print("Lower memory: ");
            print_int(mem_lower_kb());
            vga_print(" KB\n");
            vga_print("Upper memory: ");
            print_int(mem_upper_kb());
            vga_print(" KB\n");
            vga_print("Total (approx): ");
            print_int((mem_upper_kb() + 1024) / 1024);
            vga_print(" MB\n");
        }
    } else if (str_eq(input, "crash")) {
        volatile int zero = 0;
        volatile int x = 1 / zero;
        (void)x;    
    } else {
        vga_print("Unknown command: ");
        vga_print(input);
        vga_putchar('\n');
    }
}

void shell_init(void) {
    input_len = 0;
    history_pos = history_count;
    vga_print(PROMPT);
}

void shell_input(char c) {
    if (c == '\n') {
        vga_putchar('\n');
        input[input_len] = '\0';
        if (input_len > 0) {
            history_add(input);
        }
        history_pos = history_count;
        draft[0] = '\0';
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