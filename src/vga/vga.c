#include "vga.h"

void vga_write(int addr, const char* str)
{
    #define COLOR 0x0f
    #define LINE ((char*)0x0b8000)

    for(; *str; str++)
    {
        LINE[addr] = *str;
        LINE[addr + 1] = COLOR;
        addr += 2;
    }
}

void vga_write_line(int line, const char* str)
{
    vga_write(line * 80 * 2, str);
}
