# boot.s - Multiboot entry point

.set MAGIC,    0x1BADB002
.set FLAGS,    0x2
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
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
    push %ebx
    push %eax
    call kmain

    cli
hang:
    hlt
    jmp hang

.size _start, . - _start

.global idt_load
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret

.global irq0_handler
.extern timer_handler
irq0_handler:
    pusha
    call timer_handler
    popa
    iret

.global irq1_handler
.extern keyboard_handler
irq1_handler:
    pusha
    call keyboard_handler
    popa
    iret


# ---------- CPU exception handlers (vectors 0-31) ----------

.macro ISR_NOERR num
.global isr\num
isr\num:
    push $0
    push $\num
    jmp isr_common
.endm

.macro ISR_ERR num
.global isr\num
isr\num:
    push $\num
    jmp isr_common
.endm

.extern exception_handler

.section .text
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR   29
ISR_ERR   30
ISR_NOERR 31

isr_common:
    pusha
    mov %esp, %eax
    push %eax
    call exception_handler
    add $4, %esp
    popa
    add $8, %esp
    iret

.section .data
.global isr_stub_table
isr_stub_table:
.irp i,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    .long isr\i
.endr