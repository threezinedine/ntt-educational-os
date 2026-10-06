[BITS 16]
[ORG 0x7C00]

start:
    push boot_loader_success
    call print
    add esp, 2

    jmp $

print_char:
    push ebp
    mov ebp, esp

    mov al, [ebp+6]  ; Load the character to print into AL
    mov ah, 0x0E      ; BIOS teletype function
    int 0x10          ; Call BIOS to print character

    pop ebp
    ret

print:
    push ebp
    mov ebp, esp

    mov di, [ebp+6]  ; Load the character to print into AL
    xor ax, ax
    mov al, [di]
    jmp _print_loop_2

_print_loop_1:
    push ax
    call print_char
    add esp, 2
    add di, 1
    mov al, [di]
_print_loop_2:
    cmp al, 0
    jne _print_loop_1

    pop ebp
    ret

boot_loader_success: db 'Boot loader success...', 0

times 510-($-$$) db 0
dw 0xAA55