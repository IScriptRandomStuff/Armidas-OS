; These tiny stubs run before the C handler
; They fix up the stack and call isr_handler()

extern isr_handler

; Macro for exceptions WITHOUT an error code
; We push a dummy 0 so the stack layout is always the same
%macro ISR_NOERR 1
global isr%1
isr%1:
    cli                  ; disable interrupts while handling
    push byte 0          ; dummy error code
    push byte %1         ; interrupt number
    jmp isr_common
%endmacro

; Macro for exceptions WITH an error code
; CPU already pushed the error code so we just push the number
%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    push byte %1         ; interrupt number (error code already on stack)
    jmp isr_common
%endmacro

; Which exceptions push error codes automatically:
; 8, 10, 11, 12, 13, 14, 17 push error codes
; everything else does not
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
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

isr_common:
    pusha               ; push eax,ecx,edx,ebx,esp,ebp,esi,edi
    mov ax, ds
    push eax            ; save data segment

    mov ax, 0x10        ; switch to kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; push pointer to all saved registers
                        ; this becomes the struct registers *r in C

    call isr_handler    ; call our C handler

    pop eax             ; remove the esp we pushed
    pop eax             ; restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                ; restore general registers
    add esp, 8          ; clean up int_no and err_code from stack
    iret                ; return from interrupt — restores eip, cs, eflags

section .note.GNU-stack noalloc noexec nowrite progbits