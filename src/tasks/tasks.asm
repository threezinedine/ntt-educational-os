section .text

global switch_to_user_land

switch_to_user_land: ; void(uint32_t eip, uint32_t cs, uint32_t efflags, uint32_t esp, uint32_t ss)
    add esp, 4
    mov eax, [esp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    iret
