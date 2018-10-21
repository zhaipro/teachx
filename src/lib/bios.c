
#include "asm.h"
#include "bios.h"

uint8_t cmos_read(uint8_t addr)
{
    // 设置 CMOS 数据地址
    out(0x70, addr);
    // 读取刚设置的地址中的单字节数据
    return in(0x71);
}
