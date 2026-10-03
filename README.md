# VERUS

An x86 kernel mainly written in C and Assembly.

## Current Features

- x86 kernel
- C kernel source
- x86 Assembly boot code
- Makefile build system
- Linker script
- Basic keyboard input
- QEMU testing

## Versions

- v0.1 — Initial Kernel
- v0.2 — Keyboard Input

## Goals

- Hardware support
- Memory management
- Keyboard input
- An interactive shell
- Program execution
- More operating-system functionality over time

## Testing with QEMU

Verus can be tested using QEMU.

### Command

```bash
qemu-system-i386 -kernel verus.bin

 
