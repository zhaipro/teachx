
#include "asm.h"
#include "vga.h"

void start()
{
    vga_write_line(5, "Hello kernel!");
    hlt();
}
