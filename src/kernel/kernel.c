
#include "asm.h"
#include "stdio.h"
#include "vga.h"

void start()
{
    vga_set_cursor_pos(5 * 80);
    printk("%d: Hello kernel!\n", 5);
    hlt();
}