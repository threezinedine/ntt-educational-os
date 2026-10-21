[BITS 32]
[ORG 0x00]

db 'bin.hello.1', 0

start:
    xor eax, eax
    add eax, 1
    ;int 0x80
    jmp $