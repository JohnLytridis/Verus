#include "mem.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

static int valid = 0;
static unsigned int lower_kb = 0;
static unsigned int upper_kb = 0;

void mem_init(unsigned int magic, unsigned int mbi_addr) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        return;
    }
    unsigned int *mbi = (unsigned int *)mbi_addr;
    if (mbi[0] & 1) {
        lower_kb = mbi[1];
        upper_kb = mbi[2];
        valid = 1;
    }
}

int mem_valid(void) {
    return valid;
}

unsigned int mem_lower_kb(void) {
    return lower_kb;
}

unsigned int mem_upper_kb(void) {
    return upper_kb;
}