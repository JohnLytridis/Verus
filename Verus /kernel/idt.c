#include "idt.h"
#include "vga.h"

struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

/* Layout of the stack when exception_handler is called (see isr_common in boot.s) */
struct regs {
    unsigned int edi, esi, ebp, esp_unused, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags;
};

#define IDT_ENTRIES 256
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

static const char *exception_names[] = {
    "Divide by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 floating-point error",
    "Alignment check",
    "Machine check",
    "SIMD floating-point error",
    "Virtualization exception",
    "Control protection exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
    "Hypervisor injection",
    "VMM communication",
    "Security exception",
    "Reserved"
};

_Static_assert(sizeof(exception_names) / sizeof(exception_names[0]) == 32,
               "exception_names must have exactly 32 entries");

static void print_hex(unsigned int v) {
    const char *digits = "0123456789ABCDEF";
    vga_print("0x");
    for (int i = 28; i >= 0; i -= 4) {
        vga_putchar(digits[(v >> i) & 0xF]);
    }
}

void exception_handler(struct regs *r) {
    vga_set_color(0x4F);   /* white on red */
    vga_print("\n*** CPU EXCEPTION: ");
    vga_print(r->int_no < 32 ? exception_names[r->int_no] : "Unknown");
    vga_print(" ***\n\n");

    vga_print("Vector: ");     print_hex(r->int_no);
    vga_print("   Error code: "); print_hex(r->err_code);
    vga_print("\nEIP:    ");    print_hex(r->eip);
    vga_print("   CS: ");       print_hex(r->cs);
    vga_print("   EFLAGS: ");   print_hex(r->eflags);
    vga_print("\nEAX:    ");    print_hex(r->eax);
    vga_print("   EBX: ");      print_hex(r->ebx);
    vga_print("\nECX:    ");    print_hex(r->ecx);
    vga_print("   EDX: ");      print_hex(r->edx);

    if (r->int_no == 14) {
        unsigned int cr2;
        asm volatile ("mov %%cr2, %0" : "=r"(cr2));
        vga_print("\nFaulting address (CR2): ");
        print_hex(cr2);
    }

    vga_print("\n\nSystem halted.\n");
    while (1) {
        asm volatile ("cli; hlt");
    }
}

static void idt_set_gate(int num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

extern void idt_load(unsigned int);
extern unsigned int isr_stub_table[];
extern void irq0_handler(void);
extern void irq1_handler(void);

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (unsigned int)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    /* CPU exceptions: vectors 0-31 */
    for (int i = 0; i < 32; i++) {
        idt_set_gate(i, isr_stub_table[i], 0x08, 0x8E);
    }

    /* Hardware interrupts */
    idt_set_gate(32, (unsigned int)irq0_handler, 0x08, 0x8E);  /* timer */
    idt_set_gate(33, (unsigned int)irq1_handler, 0x08, 0x8E);  /* keyboard */

    idt_load((unsigned int)&idtp);
}