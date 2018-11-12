
#include "asm.h"
#include "kernel.h"
#include "stdio.h"

__attribute__((interrupt))
static void no_error_code(void *_)
{
    printk("no_error_code\n");
    hlt();
}

__attribute__((interrupt))
static void error_code(void *_, uint32_t code)
{
    printk("error_code: %d\n", code);
    hlt();
}

void init_traps()
{
    // 初始化中断向量表
    set_trap_gate(0, no_error_code);
    set_trap_gate(1, no_error_code);
    set_trap_gate(2, no_error_code);
    set_trap_gate(3, no_error_code);
    set_trap_gate(4, no_error_code);
    set_trap_gate(5, no_error_code);
    set_trap_gate(6, no_error_code);
    set_trap_gate(7, no_error_code);
    set_trap_gate(8, error_code);
    set_trap_gate(9, no_error_code);
    set_trap_gate(10, error_code);
    set_trap_gate(11, error_code);
    set_trap_gate(12, error_code);
    set_trap_gate(13, error_code);
    set_trap_gate(14, error_code);
    for (int i=15; i<256; i++)
        set_trap_gate(i, no_error_code);
    // IDT —— interrupt description table
    // 加载中断向量表
    struct idtptr_t idtptr;
    idtptr.limit = 256 * 8;
    idtptr.addr = (void*)IDT;
    lidt(&idtptr);
    printk("Init traps ok!\n");
    static int a = 0;
    a = 1 / a;
}
