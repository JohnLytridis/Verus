CC = i386-elf-gcc
AS = i386-elf-as
LD = i386-elf-ld

CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra -MMD
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot/boot.o kernel/kernel.o kernel/vga.o kernel/idt.o kernel/pic.o \
       kernel/keyboard.o kernel/shell.o kernel/pit.o kernel/mem.o

.PHONY: all run iso clean

all: verus.bin

boot/boot.o: boot/boot.s
	$(AS) --32 boot/boot.s -o boot/boot.o

kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

verus.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o verus.bin $(OBJS)

-include $(OBJS:.o=.d)

run: verus.bin
	qemu-system-i386 -kernel verus.bin -display cocoa,zoom-to-fit=on

iso: verus.bin
	cp verus.bin isodir/boot/verus.bin
	grub-mkrescue -o verus.iso isodir

clean:
	rm -f boot/*.o kernel/*.o kernel/*.d verus.bin verus.iso