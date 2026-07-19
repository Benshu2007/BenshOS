bits 32

global common_isr
global isr_stub_table

extern exception_handler
extern irq_handler          ; Create a separate C handler for PIC IRQs!

common_isr:
    pusha               ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

    ; Save segment registers safely as 32-bit values
    mov ax, ds
    movzx eax, ax       ; Clear high 16 bits of eax
    push eax
    mov ax, es
    movzx eax, ax
    push eax

    ; Load kernel data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    ; Pass the pointer to the registers state to C
    push esp
    
    ; Check if this is an exception (0-31) or an IRQ (32-47)
    ; The vector number is located at esp + 44 right now
    mov eax, [esp + 44] 
    cmp eax, 32
    jl .call_exception
    
    call irq_handler
    jmp .cleanup

.call_exception:
    call exception_handler

.cleanup:
    add esp, 4          ; Undo the 'push esp'

    ; Restore segment registers safely
    pop eax
    mov es, ax
    pop eax
    mov ds, ax

    popa                ; Restore general purpose registers
    add esp, 8          ; Clean up vector number and error code
    iretd

; --- MACROS ---

; For exceptions that do NOT push an error code (We push a fake 0)
%macro isr_no_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push 0              ; Fake error code
    push %1             ; Vector number
    jmp common_isr
%endmacro

; For exceptions that DO push a hardware error code automatically
%macro isr_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
                        ; Hardware error code is already pushed here!
    push %1             ; Vector number
    jmp common_isr
%endmacro

; --- GENERATE STUBS (0 to 47) ---

%assign i 0
%rep 32
    ; Hardware pushes error codes for 8, 10, 11, 12, 13, 14, 17, 30
    %if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17 || i == 30
        isr_err_stub i
    %else
        isr_no_err_stub i
    %endif
%assign i i+1
%endrep

; Generate IRQ Stubs (32 to 47) - None of these have error codes
%assign i 32
%rep 16
    isr_no_err_stub i
%assign i i+1
%endrep

; --- EXPANDED IDT POINTER TABLE ---
isr_stub_table:
%assign i 0 
%rep    48              ; Expanded from 32 to 48 to hold IRQs
    dd isr_stub_%+i 
%assign i i+1 
%endrep
