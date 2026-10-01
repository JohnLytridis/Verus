
.set YAHU,    0x1BADB002
.set FLAGS,    0x0
.set CHECKSUM, -(YAHU + FLAGS)

.section .multiboot
.align 4
.long YAHU
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384          
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp   
    call kmain             

    cli                     
hang:
    hlt                     
    jmp hang                

.size _start, . - _start