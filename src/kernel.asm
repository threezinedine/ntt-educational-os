section .text
global _start

extern kernel_main

_start:
    ; Your kernel code goes here
    cli
    mov esp, 0x90000
    call kernel_main