; irq_stubs.asm
; NASM, 32-bit protected mode
; Stubs for IRQs 0-15 (IDT vectors 32-47)
;
; Stack layout on entry to irq_common_stub (low → high address):
;
;   [CPU auto-push]   ss, useresp, eflags, cs, eip
;   [our push]        err_code (always 0 for IRQs), int_no
;   [pusha]           eax, ecx, edx, ebx, esp, ebp, esi, edi
;   [our push]        ds
;
; This matches struct registers in isr.h exactly.

bits 32
section .text

extern irq_handler          ; void irq_handler(struct registers *r)

; -----------------------------------------------------------------------
; Common stub — called by every irq# label below
; -----------------------------------------------------------------------
irq_common_stub:
    pusha                   ; push eax,ecx,edx,ebx,esp,ebp,esi,edi

    mov  ax, ds
    push eax                ; save data segment descriptor

    mov  ax, 0x10           ; load kernel data segment
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    push esp                ; pass pointer to struct registers as argument
    call irq_handler
    add  esp, 4             ; clean up the argument

    pop  eax                ; restore original data segment
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    popa                    ; restore general-purpose registers
    add  esp, 8             ; discard int_no and err_code
    iret                    ; restore eip, cs, eflags (and ss/esp if ring change)

; -----------------------------------------------------------------------
; IRQ stubs — each one pushes a dummy error code (0) and the vector
; number, then falls through to the common stub
; -----------------------------------------------------------------------

; Macro to keep things DRY
%macro IRQ_STUB 2           ; %1 = IRQ number (0-15), %2 = vector (32-47)
global irq%1
irq%1:
    push dword 0            ; dummy error code (IRQs never push one)
    push dword %2           ; interrupt/vector number
    jmp  irq_common_stub
%endmacro

IRQ_STUB  0,  32
IRQ_STUB  1,  33
IRQ_STUB  2,  34
IRQ_STUB  3,  35
IRQ_STUB  4,  36
IRQ_STUB  5,  37
IRQ_STUB  6,  38
IRQ_STUB  7,  39
IRQ_STUB  8,  40
IRQ_STUB  9,  41
IRQ_STUB 10,  42
IRQ_STUB 11,  43
IRQ_STUB 12,  44
IRQ_STUB 13,  45
IRQ_STUB 14,  46
IRQ_STUB 15,  47