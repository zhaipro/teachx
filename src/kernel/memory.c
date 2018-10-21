
#include "bios.h"
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

void init_memory()
{
    s_total = get_total_memory();
    printk("Total memory: %d K\n", s_total);
}
