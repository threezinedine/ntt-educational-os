section .text
global _start

extern kernel_main

_start:
    ; Your kernel code goes here
    cli
    call kernel_main