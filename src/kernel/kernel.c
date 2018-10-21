
#include "asm.h"
#include "memory.h"
#include "stdio.h"
#include "vga.h"

void start()
{
    vga_set_cursor_pos(5 * 80);
    printk("Hello kernel!\n");
    init_memory();
    hlt();
}
