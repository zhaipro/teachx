
#include <string.h>

#include "asm.h"
#include "cpu.h"
#include "settings.h"
#include "vga.h"

// 页目录的所在地址
#define page_directory _4K

static void init_page_table();

// 入口必须写在最前面，叫什么名字倒无所谓
void start()
{
    // 1. 打招呼
    // 2. 测试是否能够调用函数
    // 3. 测试增加.data
    vga_set_cursor_pos(3 * 80);
    vga_write("Hello C language!\n");
    init_page_table();
    vga_write("Hello virtual memory!\n");
    // 跳入内核
    jmp(OFFSET_KERNEL);
}

static void init_page_table()
{
    // 初始化页目录
    memset((void*)page_directory, 0, sizeof(uint32_t) * 1024);
    // 页目录也是页表
    ((uint32_t*)page_directory)[0] = page_directory | PAGE_PRESENT;
    ((uint32_t*)page_directory)[KERNEL_SEGMENT / _4M] = page_directory | PAGE_PRESENT;
    // 为了在开启分页后仍然可以继续执行初始化代码
    // 所以，前 1M 的映射方法如下
    for(int i=2; i<(_1M / _4K); i++)
    {
        ((uint32_t*)page_directory)[i] = (i * _4K) | PAGE_WRITE;
    }
    // 设置页目录的物理地址
    lcr3(page_directory);
    // 启动分页机制
    uint32_t cr0 = scr0();
    cr0 |= 0x80000000;
    lcr0(cr0);
}
