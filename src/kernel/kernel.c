
#include <string.h>

#include "asm.h"
#include "cpu.h"
#include "memory.h"
#include "settings.h"
#include "stdio.h"
#include "vga.h"

static void init();
static void initial_process();

void start()
{
    vga_set_cursor_pos(5 * 80);
    printk("Hello kernel!\n");
    init();
    initial_process();
}

static void show_gdt()
{
    struct gdtptr_t gdt;
    sgdt(&gdt);
    // 24 0x80000~
    printk("gdt.limit: %d, gdt.addr: 0x%X\n", gdt.limit, gdt.addr);
}

static void init()
{
    extern void init_traps();
    extern void init_8259A();
    extern void init_sched();
    // 陷阱的初始化需要排在前面
    init_traps();
    init_8259A();
    // 内存管理员的初始化在进程调度管理员之前
    init_memory();
    init_sched();
}

static void initial_process()
{
    show_gdt();
    extern void run();
    run();
}
