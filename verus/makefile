CC = i386-elf-gcc
AS = i386-elf-as
LD = i386-elf-ld

CFLAGS = -m32 -ffreestanding -fno-stack-protector -nostdlib -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot/boot.o kernel/kernel.o

all: verus.bin

boot/boot.o: boot/boot.s
	$(AS) --32 boot/boot.s -o boot/boot.o

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel/kernel.o

verus.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o verus.bin $(OBJS)

clean:
	rm -f boot/boot.o kernel/kernel.o verus.bin