bits 32
global _start
extern kernel_main
extern _bss_start
extern _bss_end

_start:
	mov edi, 0xB8000 + (9*80)*2
	mov esi, msg
	call print32b

    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
.zero_bss:
    test ecx, ecx
    jz .bss_done

    mov [edi], al
    inc edi
    dec ecx
    jmp .zero_bss
.bss_done:
	call kernel_main

.hang:
	cli
	hlt
	jmp .hang

msg db "Loading kernel...", 0

%include "src/asm/print.asm"
