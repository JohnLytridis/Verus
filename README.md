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
- v0.2.5 — basic shell with commands
  

## Goals

- Hardware support
- Memory management
- Keyboard input
- An interactive shell
- Program execution
- More operating-system functionality over time

## Testing
Verus can be tested using QEMU, or more simply, using a VM.

#### QEMU

##### For QEMU, you will need a C compiler for i386, and `make`
```bash
cd verus
make
qemu-system-i386 -kernel verus.bin
```

#### VMs
You can add this in any VM to try it out, using verus.iso.
 
