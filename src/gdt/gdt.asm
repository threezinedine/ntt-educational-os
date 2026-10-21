section .text

global gdt_flush
global load_tss

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]
    
    jmp 0x08:reload_cs
reload_cs:

    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret

load_tss: ;(uint32_t tss)
    mov eax, [esp + 4]
    ltr ax
    ret