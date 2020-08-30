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
    if(r) {
        printk("My child process id is %d\n", r);
    } else {
        printk("I'm a child process\n");
    }
    while(1);
}
