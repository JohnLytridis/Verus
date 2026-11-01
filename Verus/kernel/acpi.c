#include "acpi.h"
#include "vga.h"
#include "io.h"

static void print_hex(unsigned int value) {
    const char digits[] = "0123456789ABCDEF";
    vga_print("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        vga_putchar(digits[(value >> shift) & 0xF]);
    }
}

static int mem_equal(const char *a, const char *b, unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

static int checksum_ok(const unsigned char *data, unsigned int length) {
    unsigned char sum = 0;
    for (unsigned int i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum == 0;
}

struct rsdp {
    char signature[8];
    unsigned char checksum;
    char oem_id[6];
    unsigned char revision;
    unsigned int rsdt_address;
} __attribute__((packed));

static struct rsdp *find_rsdp_in(unsigned int start, unsigned int end) {
    for (unsigned int addr = start; addr + sizeof(struct rsdp) <= end; addr += 16) {
        const char *p = (const char *)addr;
        if (mem_equal(p, "RSD PTR ", 8) &&
            checksum_ok((const unsigned char *)p, sizeof(struct rsdp))) {
            return (struct rsdp *)addr;
        }
    }
    return 0;
}

static struct rsdp *find_rsdp(void) {
    unsigned int ebda = (*(unsigned short *)0x40E) << 4;
    if (ebda >= 0x80000 && ebda < 0xA0000) {
        struct rsdp *found = find_rsdp_in(ebda, ebda + 1024);
        if (found) {
            return found;
        }
    }
    return find_rsdp_in(0xE0000, 0x100000);
}

struct sdt_header {
    char signature[4];
    unsigned int length;
    unsigned char revision;
    unsigned char checksum;
    char oem_id[6];
    char oem_table_id[8];
    unsigned int oem_revision;
    unsigned int creator_id;
    unsigned int creator_revision;
} __attribute__((packed));

static struct sdt_header *find_table(struct rsdp *rsdp, const char *signature) {
    struct sdt_header *rsdt = (struct sdt_header *)rsdp->rsdt_address;
    if (!mem_equal(rsdt->signature, "RSDT", 4)) {
        return 0;
    }

    unsigned int count = (rsdt->length - sizeof(struct sdt_header)) / 4;
    unsigned int *entries = (unsigned int *)((unsigned int)rsdt + sizeof(struct sdt_header));

    for (unsigned int i = 0; i < count; i++) {
        struct sdt_header *table = (struct sdt_header *)entries[i];
        if (mem_equal(table->signature, signature, 4) &&
            checksum_ok((const unsigned char *)table, table->length)) {
            return table;
        }
    }
    return 0;
}

struct fadt {
    struct sdt_header header;
    unsigned int firmware_ctrl;
    unsigned int dsdt;
    unsigned char reserved;
    unsigned char preferred_pm_profile;
    unsigned short sci_interrupt;
    unsigned int smi_command_port;
    unsigned char acpi_enable;
    unsigned char acpi_disable;
    unsigned char s4bios_req;
    unsigned char pstate_control;
    unsigned int pm1a_event_block;
    unsigned int pm1b_event_block;
    unsigned int pm1a_control_block;
    unsigned int pm1b_control_block;
} __attribute__((packed));

static int find_s5(struct fadt *fadt, unsigned char *typ_a, unsigned char *typ_b) {
    struct sdt_header *dsdt = (struct sdt_header *)fadt->dsdt;
    if (!mem_equal(dsdt->signature, "DSDT", 4)) {
        return 0;
    }

    unsigned char *start = (unsigned char *)dsdt;
    for (unsigned int i = 2; i + 8 < dsdt->length; i++) {
        if (!mem_equal((const char *)start + i, "_S5_", 4)) {
            continue;
        }
        int name_ok = start[i - 1] == 0x08 ||
                      (start[i - 2] == 0x08 && start[i - 1] == '\\');
        if (!name_ok || start[i + 4] != 0x12) {
            continue;
        }

        unsigned char *p = start + i + 5;
        p += ((*p & 0xC0) >> 6) + 2;
        if (*p == 0x0A) p++;
        *typ_a = *p;
        p++;
        if (*p == 0x0A) p++;
        *typ_b = *p;
        return 1;
    }
    return 0;
}

void acpi_info(void) {
    struct rsdp *rsdp = find_rsdp();
    if (rsdp == 0) {
        vga_print("ACPI: RSDP not found\n");
        return;
    }
    vga_print("RSDP at ");
    print_hex((unsigned int)rsdp);
    vga_print("\n");

    struct fadt *fadt = (struct fadt *)find_table(rsdp, "FACP");
    if (fadt == 0 || fadt->header.length < sizeof(struct fadt)) {
        vga_print("ACPI: FADT not found\n");
        return;
    }
    vga_print("PM1a control port: ");
    print_hex(fadt->pm1a_control_block);
    vga_print("\nPM1b control port: ");
    print_hex(fadt->pm1b_control_block);
    vga_print("\nSMI command port:  ");
    print_hex(fadt->smi_command_port);
    vga_print("\nACPI enable value: ");
    print_hex(fadt->acpi_enable);
    vga_print("\n");

    unsigned char typ_a;
    unsigned char typ_b;
    if (find_s5(fadt, &typ_a, &typ_b)) {
        vga_print("Sleep type A: ");
        print_hex(typ_a);
        vga_print("   B: ");
        print_hex(typ_b);
        vga_print("\n");
    } else {
        vga_print("ACPI: _S5_ not found\n");
    }
}

static void delay(void) {
    for (volatile unsigned int i = 0; i < 20000000; i++) {
    }
}

static void try_acpi_shutdown(void) {
    struct rsdp *rsdp = find_rsdp();
    if (rsdp == 0) {
        vga_print("ACPI: no RSDP\n");
        return;
    }

    struct fadt *fadt = (struct fadt *)find_table(rsdp, "FACP");
    if (fadt == 0 || fadt->header.length < sizeof(struct fadt)) {
        vga_print("ACPI: no FADT\n");
        return;
    }

    unsigned char typ_a;
    unsigned char typ_b;
    if (!find_s5(fadt, &typ_a, &typ_b)) {
        vga_print("ACPI: no _S5_\n");
        return;
    }

    unsigned short pm1a = (unsigned short)fadt->pm1a_control_block;
    unsigned short pm1b = (unsigned short)fadt->pm1b_control_block;
    if (pm1a == 0) {
        vga_print("ACPI: no PM1a port\n");
        return;
    }

    if ((inw(pm1a) & 1) == 0 && fadt->smi_command_port != 0 && fadt->acpi_enable != 0) {
        outb((unsigned short)fadt->smi_command_port, fadt->acpi_enable);
        for (int i = 0; i < 300 && (inw(pm1a) & 1) == 0; i++) {
            delay();
        }
    }

    outw(pm1a, (unsigned short)((typ_a << 10) | (1 << 13)));
    if (pm1b != 0) {
        outw(pm1b, (unsigned short)((typ_b << 10) | (1 << 13)));
    }
}

void acpi_shutdown(void) {
    try_acpi_shutdown();

    // If we are still running, the proper way did not work.
    delay();
    vga_print("ACPI shutdown failed, trying emulator ports...\n");
    outw(0x604, 0x2000);    // QEMU
    outw(0xB004, 0x2000);   // older QEMU / Bochs
    outw(0x4004, 0x3400);   // VirtualBox
}
