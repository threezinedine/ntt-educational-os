section .text

global prepare_user_land
global switch_to_user_land
global internal_switch_to_user_land

prepare_user_land: ; uint32_t(uint32_t* pKernel_esp, uint32_t eip, uint32_t cs, uint32_t efflags, uint32_t esp, uint32_t ss)
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8] ; pKernel_esp
    mov edx, [eax] ; kernel_esp

    mov eax, [ebp + 28] ; ss
    mov [edx - 4], eax
    
    mov eax, [ebp + 24] ; esp
    mov [edx - 8], eax
    
    mov eax, [ebp + 20] ; efflags
    mov [edx - 12], eax
    
    mov eax, [ebp + 16] ; cs
    mov [edx - 16], eax
    
    mov eax, [ebp + 12] ; eip
    mov [edx - 20], eax

    mov eax, edx
    sub eax, 20
    mov ebx, [ebp + 8]; pKernel_esp
    mov [ebx], eax
    
    pop ebp
    ret

switch_to_user_land: ; void(uint32_t kernel_esp)
    add esp, 4
    mov eax, [esp]
    mov esp, eax
    mov eax, [esp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    iret

internal_switch_to_user_land: ; void(uint32_t kernel_esp, uint32_t* p_prev_kernel_esp)
    add esp, 4

    ; save current esp to *p_prev_kernel_esp
    mov eax, [esp + 4] ; p_prev_kernel_esp
    mov [eax], esp

    mov eax, [esp]
    mov esp, eax
    mov eax, [esp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    iret
