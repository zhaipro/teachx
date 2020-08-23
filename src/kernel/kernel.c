
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

static void init_idt()
{
    // IDT —— interrupt description table
    // 加载中断向量表
    memset((void*)IDT, 0, 256 * 8);
    struct idtptr_t idtptr;
    idtptr.limit = 256 * 8;
    idtptr.addr = (void*)IDT;
    lidt(&idtptr);
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
    extern void init_8259A();
    extern void init_sched();
    extern void init_ipc();
    extern void shell_process();
    init_idt();
    init_8259A();
    // 内存管理员的初始化在进程调度管理员之前
    init_memory();
    init_sched(shell_process);
    init_ipc();
}

static void initial_process()
{
    show_gdt();
    extern void run();
    run();
}
