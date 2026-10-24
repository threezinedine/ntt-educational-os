[BITS 32]
[ORG 0x08000000]

db 'bin.hello.1', 0

start:
    sub esp, 4 ; int i
    xor eax, eax
    mov [esp], eax

    mov ebx, counting_program
    mov eax, 2
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

    push 0x1fffffff
    call delay_1ms
    add esp, 4

    mov eax, [esp]

end_loop:
    cmp eax, 10
    jl main_loop

    jmp $

message:
db 10, 'Counting from hello: ', 0

counting_program:
db 'counting', 0

delay_1ms:
    sub esp, 4
    mov eax, 0
    mov [esp], eax

    jmp _delay_1ms_end_loop
    
_delay_1ms_main_loop:
    add eax, 1

_delay_1ms_end_loop:
    mov ebx, [esp + 8]
    cmp eax, ebx
    jl _delay_1ms_main_loop

    add esp, 4
    ret