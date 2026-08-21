/*
 * Linker script for MM32F0144C6P
 * 64 KiB flash @ 0x08000000, 8 KiB SRAM @ 0x20000000
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 0x10000
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x02000
}

__top_of_sram = ORIGIN(SRAM) + LENGTH(SRAM);
__stack_size  = 0x400;

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT > FLASH
    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    .heap :
    {
        . = ALIGN(4);
        _sheap = .;
        . = . + 0x200;
        . = ALIGN(4);
        _eheap = .;
    } > SRAM

    __stack_top = __top_of_sram;
    __initial_sp = __top_of_sram;

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}