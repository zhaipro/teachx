
#include "asm.h"
#include "vga.h"

#define CTRLC_ADDR_PORT 0x3d4
#define CTRLC_DATA_PORT 0x3d5
#define CTRLC_INDEX_CURSOR_H 0xe
#define CTRLC_INDEX_CURSOR_L 0xf

void vga_set_cursor_pos(int pos)
{
    out(CTRLC_ADDR_PORT, CTRLC_INDEX_CURSOR_H);
    out(CTRLC_DATA_PORT, pos >> 8);
    out(CTRLC_ADDR_PORT, CTRLC_INDEX_CURSOR_L);
    out(CTRLC_DATA_PORT, pos & 0xff);
}

int vga_write(int addr, const char* str)
{
    #define COLOR 0x0f
    #define LINE ((char*)0x0b8000)

    for(; *str; str++)
    {
        LINE[addr] = *str;
        LINE[addr + 1] = COLOR;
        addr += 2;
    }
    vga_set_cursor_pos(addr >> 1);
    return addr;
}

void vga_write_line(int line, const char* str)
{
    vga_write(line * 80 * 2, str);
}
