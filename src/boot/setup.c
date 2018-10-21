
#include "asm.h"
#include "vga.h"

// 入口必须写在最前面，叫什么名字倒无所谓
void start()
{
    // 1. 打招呼
    // 2. 测试是否能够调用函数
    // 3. 测试增加.data
    vga_write_line(3, "Hello C language!");
    hlt();
}
