#include "asm.h"
#include "ipc.h"
#include "stdio.h"

void shell_process()
{
    printk("Shell process ...\n");
    // 测试用，发送一下当前日期
    struct msg_t msg;
    int r;
    msg.p0 = 2018;
    msg.p1 = 11;
    msg.p2 = 4;
    r = system_call(-1, 0, (uint32_t)&msg);
    printk("return value: %d\n", r);
    msg.p0 = 2020;
    msg.p1 = 8;
    msg.p2 = 29;
    r = system_call(-1, 0, (uint32_t)&msg);
    printk("return value: %d\n", r);
    while(1);
}
