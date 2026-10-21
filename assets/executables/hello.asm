[BITS 32]
[ORG 0x08000000]

db 'bin.hello.1', 0

start:
    sub esp, 4 ; int i
    xor eax, eax
    mov [esp], eax

    mov ebx, counting_program
    mov eax, 3
    int 0x80

    mov eax, 0

    jmp end_loop
    
main_loop:
    add eax, 1
    mov [esp], eax

    mov eax, 0
    mov ebx, message
    int 0x80
    
    mov ebx, [esp]
    mov eax, 1
    int 0x80

    mov eax, 2
    int 0x80

    mov eax, [esp]

end_loop:
    cmp eax, 10
    jl main_loop

    jmp $

message:
db 10, 'Counting from hello: ', 0

counting_program:
db 'counting'