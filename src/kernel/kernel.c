
#include "asm.h"
#include "vga.h"

void start()
{
    vga_set_cursor_pos(5 * 80);
    vga_write("Hello kernel!\n");
    hlt();
}
