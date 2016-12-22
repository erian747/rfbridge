/*
*****************************************************************************
**
**  File        : stm32_512_48.ld
**
**  Abstract    : Linker script for STM32 devies with at least 512Kb Flash and 48kb RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**
**  Target      : STMicroelectronics STM32
**
**  Environment : Atollic TrueSTUDIO(R)
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
*****************************************************************************
*/

/* Entry Point */


/* Highest address of the user mode stack */
_estack = 0x20001000;   

/* Specify the memory areas */
MEMORY
{
  CODE (rx)       : ORIGIN = 0x08000000, LENGTH = 0x7800
  CONFIG (rx)     : ORIGIN = 0x08007800, LENGTH = 0x800   
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 0x1000 
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}

/* Define output sections */
SECTIONS
{
  /* The vector table shall be placed first */
  .vectors :
  {
    . = ALIGN(4);
    KEEP(*(.vectors)) /* Vector table code */
    . = ALIGN(4);
  } >CODE

  /* The program code and other data goes into CODE */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
    _etext = .;        /* define a global symbol at end of code */
  } >CODE

  .ARM.attributes : 
  { 
    *(.ARM.attributes) 
    . = ALIGN(4);
  } > CODE
  
  /* used by the startup to initialize data */
  _sidata = .;
  
  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : AT ( _sidata )
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  PROVIDE ( end = _ebss );
  PROVIDE ( _end = _ebss );
  PROVIDE (end = .); 
 
  /* Remove information from the standard libraries */

  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }
}
