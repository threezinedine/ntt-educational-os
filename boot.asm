[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x9000 ; keep the stack clear of the kernel image loaded at 0x500
    sti

    push gdt_setup_message
    call print
    add esp, 2

    sti

    push 0x0500 ; address
    push 56     ; sectors count (0x500 + 56*512 must stay below 0x7C00)
    push 0x02   ; sector index
    push 0x00   ; head
    push 0x00   ; cylinder
    call load_sectors
    add esp, 10

    cli

    lgdt [gdt_descriptor]

    push gdt_setup_complete_message
    call print
    add esp, 2

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:protected_mode

gdt_start:
gdt_null: 
    dq 0  ; Null descriptor for GDT
gdt_kernel_code:
    dw 0xffff ; Limit low: 0xFFFF
    dw 0x0000 ; Base low: 0x0000
    db 0x00   ; Base middle: 0x00
    db 0x9A   ; Present: 1, DPL: 0, Code or Data: 1, Executable: 1, Readable: 1, Accessed: 0
    db 0xCF   ; Granularity: 1, DB: 1, Long Mode: 0, Reversed: 0, Limit high: 0xF
    db 0x00   ; Base high: 0x00

gdt_kernel_data:
    dw 0xffff ; Limit low: 0xFFFF
    dw 0x0000 ; Base low: 0x0000
    db 0x00   ; Base middle: 0x00
    db 0x92   ; Present: 1, DPL: 0, Code or Data: 1, Executable: 0, Writable: 1, Accessed: 0
    db 0xCF   ; Granularity: 1, DB: 1, Long Mode: 0, Reversed: 0, Limit high: 0xF
    db 0x00   ; Base high: 0x00

gdt_user_code:
    dw 0xffff ; Limit low: 0xFFFF
    dw 0x0000 ; Base low: 0x0000
    db 0x00   ; Base middle: 0x00
    db 0xFA   ; Present: 1, DPL: 3, Code or Data: 1, Executable: 1, Readable: 1, Accessed: 0
    db 0xCF   ; Granularity: 1, DB: 1, Long Mode: 0, Reversed: 0, Limit high: 0xF
    db 0x00   ; Base high: 0x00

gdt_user_data:
    dw 0xffff ; Limit low: 0xFFFF
    dw 0x0000 ; Base low: 0x0000
    db 0x00   ; Base middle: 0x00
    db 0xF2   ; Present: 1, DPL: 3, Code or Data: 1, Executable: 0, Writable: 1, Accessed: 0
    db 0xCF   ; Granularity: 1, DB: 1, Long Mode: 0, Reversed: 0, Limit high: 0xF
    db 0x00   ; Base high: 0x00

gdt_task_state: ; needa rewrite later
    dw 0xffff ; Limit low: 0xFFFF
    dw 0x0000 ; Base low: 0x0000
    db 0x00   ; Base middle: 0x00
    db 0x89   ; Present: 1, DPL: 0, Code or Data: 1, Executable: 0, Writable: 1, Accessed: 1
    db 0x0F   ; Granularity: 0, DB: 0, Long Mode: 0, Reversed: 0, Limit high: 0xF
    db 0x00   ; Base high: 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


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

load_sectors: ; (cylinder, head, sector, sectors count, start address)
    push ebp
    mov ebp, esp

    push disk_load_message
    call print
    add esp, 2

    mov bx, [ebp+14]
    mov ah, 0x02
    mov al, [ebp+12]  ; sectors count
    mov ch, [ebp+6] ; cylinder number
    mov cl, [ebp+10] ; sector index
    mov dh, [ebp+8] ; head number

    int 0x13          ; Call BIOS to load the sector
    jc on_disk_load_failure

    push disk_load_success_message
    call print
    add esp, 2

    pop ebp
    ret

on_disk_load_failure:
    push disk_load_failure_message
    call print
    add esp, 2
    jmp $  ; Hang the system for now

gdt_setup_message: db 'GDT setting up...', 0x0A, 0x0D, 0
gdt_setup_complete_message: db 'GDT setup complete.', 0x0A, 0x0D, 0
disk_load_message: db 'Loading disk sector...', 0x0A, 0x0D, 0
disk_load_success_message: db 'Disk sector loaded successfully.', 0x0A, 0x0D, 0
disk_load_failure_message: db 'Failed to load disk sector.', 0x0A, 0x0D, 0

[BITS 32]
protected_mode:
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    jmp 0x500

times 510-($-$$) db 0
dw 0xAA55
