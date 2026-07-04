[org 0x7C00]
bits 16
jmp start

start:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7B00
	sti

	mov [boot_drive], dl ; save the boot drive
	mov si, msg
	call print16b

	; Load kernel
	call load_kernel

	call enable_a20

   	call check_elf

   	call get_entry

   	call load_segments

	call enter_protected_mode

load_kernel:
    xor ax, ax
    mov es, ax
    mov bx, ELF_LOAD_ADDR

	mov ah, 0x02 ; BIOS function: read sectors
	mov al, 30 ; number of sectors to reads

	; disk details
	mov ch, 0 ; cylinder
	mov cl, 2 ; sector number
	mov dh, 0 ; head number

	mov dl, [boot_drive]
	int 0x13 ; BIOS disk service

	jc disk_error
	ret

check_elf:
    mov si, ELF_LOAD_ADDR

    mov al, [si+0]
    cmp al, 0x7F
    jne bad_elf

    mov al, [si+1]
    cmp al, 'E'
    jne bad_elf

    mov al, [si+2]
    cmp al, 'L'
    jne bad_elf

    mov al, [si+3]
    cmp al, 'F'
    jne bad_elf

    ret

bad_elf:
    mov si, err
    call print16b
    jmp $

get_entry:
    mov eax, [ELF_LOAD_ADDR + 0x18]
    mov [entry_point], eax
    ret

load_segments:
    mov si, ELF_LOAD_ADDR

    mov eax, [si + 0x1C]
    add eax, ELF_LOAD_ADDR
    mov esi, eax

    mov cx, [ELF_LOAD_ADDR + 0x2C]
.ph_loop:
    push cx

    mov eax, [esi]
    cmp eax, 1
    jne .next

    ; copy segment
    mov edi, [esi + 0x08]
    mov ebx, [esi + 0x04]
    add ebx, ELF_LOAD_ADDR

    mov ecx, [esi + 0x10]
    test ecx, ecx
    jz .next
.copy:
    mov al, [ebx]
    mov [edi], al
    inc ebx
    inc edi
    dec ecx
    jnz .copy
.next:
    add esi, 0x20
    pop cx
    loop .ph_loop

    ret

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al

    ; --- enter unreal mode ---
    push ds
    push es
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp $+2          ; flush prefetch queue
    mov bx, 0x10
    mov ds, bx
    mov es, bx
    and al, 0xFE
    mov cr0, eax      ; back to real mode, but...
    pop es
    pop ds            ; ...DS/ES descriptor *limits* stay 4GB (cached)
    ret

enter_protected_mode:
    cli

    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:pm_entry

gdt_start:
    dq 0x0000000000000000     ; null descriptor
    dq 0x00CF9A000000FFFF     ; code segment
    dq 0x00CF92000000FFFF     ; data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

disk_error:
	mov si, err
	call print16b
	jmp $

bits 32
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000

    mov eax, [entry_point]
    jmp eax

clear:
    push edi
    push ecx
    push ax

    mov edi, 0xB8000
    mov ecx, 80*25
    mov ax, 0x0F20
.clear_loop:
    mov [edi], ax
    add edi, 2
    loop .clear_loop

    pop ax
    pop ecx
    pop edi
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Include's Area ;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;
%include "src/asm/print.asm"

;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;; Data's Area ;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;
msg db "Booting kernel...", 0x0A, 0x0D, 0

boot_drive db 0
entry_point dd 0
ELF_LOAD_ADDR equ 0x1000

err db "Disk error!", 0x0A, 0x0D, 0


;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; Bootloader Foot ;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;
times 510-($-$$) db 0
dw 0xAA55