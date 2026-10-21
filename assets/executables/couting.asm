[BITS 32]
[ORG 0x08000000]

db 'bin.counting.1', 0

start:
    sub esp, 4 ; int i
    sub esp, 4 ; int sum
    xor eax, eax
    mov [esp + 4], eax
    mov [esp], eax
    jmp end_loop
    
main_loop:
    add eax, 1
    mov [esp + 4], eax

    add eax, [esp]
    mov [esp], eax
    
    mov eax, 0
    mov ebx, message
    int 0x80
    
    mov ebx, [esp]
    mov eax, 1
    int 0x80

    mov eax, 2
    int 0x80

    mov eax, [esp + 4]

end_loop:
    cmp eax, 10
    jl main_loop

    mov ebx, finish_message
    mov eax, 0
    int 0x80

    jmp $

message:
    db 10, 'Current value: ', 0

finish_message:
    db 10, 'Done!', 0
