section .text

global outb
global inb
global insw
global outsw

outb:
    mov dx, [esp+4]
    mov al, [esp+8]
    out dx, al
    ret

inb:
    mov dx, [esp+4]
    in al, dx
    movzx eax, al
    ret

insw: ; (uint16_t port, uint32_t* buffer, uint32_t count)
    push edi
    cld
    mov dx, [esp+8]
    mov edi, [esp+12]
    mov ecx, [esp+16]
    rep insw
    pop edi
    ret

outsw: ; (uint16_t port, uint32_t* buffer, uint32_t count)
    push edi
    cld
    mov dx, [esp+8]
    mov edi, [esp+12]
    mov ecx, [esp+16]
    rep outsw
    pop esi
    ret