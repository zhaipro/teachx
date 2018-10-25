
#include <string.h>

#include "asm.h"
#include "bios.h"
#include "cpu.h"
#include "kernel.h"
#include "settings.h"
#include "stdio.h"

static uint32_t s_total;

// 获取内存总量，单位：K
static uint32_t get_total_memory()
{
    uint32_t total = cmos_read(0x18);
    total <<= 8;
    total |= cmos_read(0x17);
    return total;
}

static uint32_t get_free_page()
{
    // 内存管理员只负责管理 1M 以后的内存，之前的交给内核
    static uint32_t s_start = _1M / _4K;
    return (s_start++) * _4K;
}

// https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html
__attribute__((interrupt)) static void page_fault(void *_, uint32_t error_code)
{
    static int time = 0;
    if (time == 0)
        printk("Hello interrupt\n");
    printk("page fault, error: %d, addr: %d\n", error_code, scr2());
    time ++;
    if (time >= 3)
        hlt();
}

static void init_pdt()
{
    // 页目录(Page Directory)
    uint32_t pd = get_free_page();
    // 页表(page table)
    uint32_t pt0 = get_free_page();
    printk("pd: %d, pt0: %d\n", pd, pt0);
    // 先清空
    memset((void*)pd, 0, 2 * sizeof(uint32_t) * 1024);
    // 前 4M 的映射
    ((uint32_t*)pd)[0] = pt0 | PAGE_PRESENT;
    // [0, 4K) 不可用，用于声明空指针
    // [4K, 1M) 指向真实的地址，用于书写 vga
    // [1M, 4M) 不用
    for (int i=1; i<_1M/_4K; i++) {
        ((uint32_t*)pt0)[i] = (i * _4K) | PAGE_PRESENT;
    }
    // 页目录也是页表
    ((uint32_t*)pd)[PDT / _4M] = pd | PAGE_PRESENT;
    // 系统段的前 4M 与 [0, 4M) 相同
    ((uint32_t*)pd)[KERNEL_SEGMENT / _4M] = pt0 | PAGE_PRESENT;
    // 设置页目录的物理地址
    lcr3(pd);
}

void init_memory()
{
    s_total = get_total_memory();
    printk("Total memory: %d K\n", s_total);
    init_pdt();
    // 加载页故障修理工
    set_trap_gate(INT_VECTOR_PAGE_FAULT, page_fault);
    printk("Init memory ok!\n");
    // 测试内存管理员是否能发现我犯下的错误
    ((int*)0)[1] = 0;
}
