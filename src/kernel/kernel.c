
#include <string.h>

#include "asm.h"
#include "cpu.h"
#include "memory.h"
#include "settings.h"
#include "stdio.h"
#include "vga.h"

static void init();

void start()
{
    vga_set_cursor_pos(5 * 80);
    printk("Hello kernel!\n");
    init();
    init_memory();
    hlt();
}

static void init()
{
    // IDT —— interrupt description table
    // 加载中断向量表
    memset((void*)IDT, 0, 256 * 8);
    struct idtptr_t idtptr;
    idtptr.limit = 256 * 8;
    idtptr.addr = (void*)IDT;
    lidt(&idtptr);
}
