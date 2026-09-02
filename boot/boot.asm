; boot/boot.asm
MBOOT_MAGIC    equ 0x1BADB002
MBOOT_ALIGN    equ 1 << 0         ; Align modules on page boundaries
MBOOT_MEMINFO  equ 1 << 1         ; Provide memory map
MBOOT_FLAGS    equ MBOOT_ALIGN | MBOOT_MEMINFO
MBOOT_CHECKSUM equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    dd MBOOT_MAGIC
    dd MBOOT_FLAGS
    dd MBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384          ; 16 KB stack
stack_top:

section .text
global _start
extern kernel_main      ; defined in kernel.c

_start:
    mov esp, stack_top  ; set up stack
    call kernel_main    ; jump into C
    hlt                 ; halt if kernel_main returns

    section .note.GNU-stack noalloc noexec nowrite progbits