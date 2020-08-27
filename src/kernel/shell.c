#include "asm.h"
#include "ipc.h"
#include "stdio.h"

void shell_process()
{
    printk("Shell process ...\n");
    // 测试用，发送一下当前日期
    struct msg_t msg;
    msg.p0 = 2018;
    msg.p1 = 11;
    msg.p2 = 4;
    system_call(-1, 0, (uint32_t)&msg);
    msg.p0 = 2020;
    msg.p1 = 8;
    msg.p2 = 29;
    system_call(-1, 0, (uint32_t)&msg);
    while(1);
}
