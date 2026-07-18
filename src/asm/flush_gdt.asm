bits 32
global flush_gdt

flush_gdt:
    ; args using cdecl are received from stackin esp + 4
    mov eax, [esp + 4] 
    lgdt [eax]

    ; relaod data segment registers
    mov ax, 0x10 ; kernel data selector

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; reload code segment
    ; 0x08 is the kernel code segment.
    jmp 0x08:.flush

.flush:
    ret
