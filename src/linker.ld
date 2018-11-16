SECTIONS
{
    .text BLOCK(4) : { *(.text) }
    .data BLOCK(4) : { *(.data) }
    .rdata BLOCK(4) : { *(.rdata) }
}
