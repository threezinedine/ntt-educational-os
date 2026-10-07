OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)
SECTIONS
{
    . = 0x0500;
    
    .text :
    {
        */kernel.asm.o(.text)
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }

    .rodata :
    {
        *(.rodata)
    }

    .comment :
    {
        *(.comment)
    }
}