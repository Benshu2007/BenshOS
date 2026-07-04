print16b:
    pusha
    push si
    push ds

    xor ax, ax
    mov ds, ax

    mov ah, 0x0E
.loop:
    lodsb
    or al, al
    jz .done
	
    int 0x10
    jmp .loop
.done:
    pop ds
    pop si
    popa

    ret

print32b:
    push eax
.loop:
    mov al, [esi]
    test al, al
    jz .done

    mov ah, 0x07
    mov [edi], ax
    add edi, 2
    inc esi
    jmp .loop
.done:
    pop eax
    ret
