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

insw: ; (uint16_t port, uint32_t* buffer)
    cld
    mov dx, [esp+4]
    mov esi, [esp+8]
    mov ecx, [esp+12]
    rep insd
    ret

outsw: ; (uint16_t port, uint32_t* buffer)
    cld
    mov dx, [esp+4]
    mov esi, [esp+8]
    mov ecx, [esp+12]
    rep outsd
    ret