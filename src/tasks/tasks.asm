section .text

global prepare_user_land
global switch_to_user_land
global internal_switch_to_user_land
global task_switch

extern s_task_head
extern s_current_task
extern task_next_task
extern page_switch_page_directory
extern set_tss_esp0

prepare_user_land: ; uint32_t(uint32_t* pKernel_esp, uint32_t eip, uint32_t cs, uint32_t efflags, uint32_t esp, uint32_t ss)
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8] ; pKernel_esp
    mov edx, [eax] ; kernel_esp

    ; iret frame
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

    ; popa frame (eax, ecx, edx, ebx, esp, ebp, esi, edi)
    mov dword [edx - 24], 0 ; eax
    mov dword [edx - 28], 0 ; ecx
    mov dword [edx - 32], 0 ; edx
    mov dword [edx - 36], 0 ; ebx
    mov dword [edx - 40], 0 ; esp (ignored by popa)
    mov dword [edx - 44], 0 ; ebp
    mov dword [edx - 48], 0 ; esi
    mov dword [edx - 52], 0 ; edi

    mov eax, edx
    sub eax, 52
    mov ebx, [ebp + 8]; pKernel_esp
    mov [ebx], eax
    
    pop ebp
    ret

switch_to_user_land: ; void(uint32_t kernel_esp)
    mov esp, [esp + 4]
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    iret

task_switch: ; void()
    pusha

    ; Send End of Interrupt (EOI) to the PIC
    mov al, 0x20
    out 0x20, al

    ; Save current task's ESP before switching
    mov eax, [s_current_task]
    test eax, eax
    jz .skip_save
    mov [eax + 4], esp ; eax + 4 is kernel_esp
.skip_save:

    call task_next_task

    cmp eax, 0
    je task_switch_end

    mov eax, s_current_task 
    mov eax, [eax]
    mov eax, [eax + 12] ; page_directory
    push eax
    call page_switch_page_directory
    add esp, 4
    mov ebx, [s_current_task]

    ; mov eax, s_current_task
    ; mov eax, [eax]
    ; mov eax, [eax + 4] ; kernel_esp
    ; ; Switch page directory
    ; mov eax, [ebx + 12] ; page_directory
    ; mov cr3, eax

    ; Update TSS esp0 to next task's kernel_stack_top (offset 8)
    mov eax, [ebx + 8] ; kernel_stack_top
    push eax
    call set_tss_esp0
    add esp, 4

    mov eax, s_current_task
    mov eax, [eax]
    mov eax, [eax + 4] ; kernel_esp
    mov esp, eax
    ; Switch stack pointer to next task's kernel_esp
    mov esp, [ebx + 4] ; kernel_esp

task_switch_end:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    iret