/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(_elf_start)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions */
PHDRS
{
    null    PT_NULL    FLAGS(0) ;                   /* Null segment */
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
}

SECTIONS
{
    /* We wanna be placed in the topmost 2GiB of the address space, for optimisations */
    /* and because that is what the stivale2 spec mandates. */
    /* Any address in this region will do, but often 0xffffffff80000000 is chosen as */
    /* that is the beginning of the region. */
    . = 0xffffffff80000000;
    _start_of_kernel = .;
    _start_of_text = .;
    .text : {
        *(.text .text.*)
    } :text
    _end_of_text = .;

    /* Move to the next memory page for .rodata */
    . += CONSTANT(MAXPAGESIZE);

    /* We place the .stivale2hdr section containing the header in its own section, */
    /* and we use the KEEP directive on it to make sure it doesn't get discarded. */
    .stivale2hdr : {
        KEEP(*(.stivale2hdr))
    } :rodata

    _start_of_rodata = .;
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata
    _end_of_rodata = .;

    /* Move to the next memory page for .data */
    . += CONSTANT(MAXPAGESIZE);
    _start_of_data = .;
    .data : {
        *(.data .data.*)
    } :data
    _end_of_data = .;

    _start_of_bss = .;
    .bss : {
        _bss = .;
        *(COMMON)
        *(.bss .bss.*)
        _bss_end = .;
    } :data
    _end_of_bss = .;
    _end_of_kernel = .;
}
