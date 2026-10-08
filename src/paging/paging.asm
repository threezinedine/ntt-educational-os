section .text

global paging_enable_paging
global paging_switch_page_directory

paging_enable_paging: ;()
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    xor eax, eax
    ret

paging_switch_page_directory: ;(void* page_directory)
    mov eax, [esp+4]
    mov cr3, eax
    xor eax, eax
    ret