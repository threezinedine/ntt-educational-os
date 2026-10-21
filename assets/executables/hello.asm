[BITS 32]
[ORG 0x08000000]

db 'bin.hello.1', 0

start:
    xor eax, eax
    mov ebx, message
    mov eax, 0
    int 0x80
    jmp $

message:
db 'Hello from Userland!', 0
