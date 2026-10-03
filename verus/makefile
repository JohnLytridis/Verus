CC = i386-elf-gcc
AS = i386-elf-as
LD = i386-elf-ld

CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot/boot.o kernel/kernel.o kernel/vga.o kernel/idt.o kernel/pic.o kernel/keyboard.o

all: verus.bin

boot/boot.o: boot/boot.s
	$(AS) --32 boot/boot.s -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel/kernel.o

kernel/vga.o: kernel/vga.c kernel/vga.h
	$(CC) $(CFLAGS) -c kernel/vga.c -o kernel/vga.o

kernel/idt.o: kernel/idt.c kernel/idt.h
	$(CC) $(CFLAGS) -c kernel/idt.c -o kernel/idt.o

kernel/pic.o: kernel/pic.c kernel/pic.h
	$(CC) $(CFLAGS) -c kernel/pic.c -o kernel/pic.o

kernel/keyboard.o: kernel/keyboard.c kernel/keyboard.h
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o kernel/keyboard.o

verus.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o verus.bin $(OBJS)

clean:
	rm -f boot/boot.o kernel/kernel.o kernel/vga.o kernel/idt.o kernel/pic.o kernel/keyboard.o verus.bin