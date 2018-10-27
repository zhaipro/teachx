
#include "asm.h"
#include "kernel.h"
#include "stdio.h"
#include "i8259.h"

__attribute__((interrupt)) static void timer_int(void *_)
{
    static int s_time = 0;
    s_time ++;
    if (s_time % 10000 == 0)
        printk("Hello timer!\n");
    eoi_m();
}

void init_timer()
{
    // 初始化定时器
    #define PORT_CLOCK_MODE 0X43
    out_p(PORT_CLOCK_MODE, 0x36);
    // 设置计数器0
    #define PORT_CLOCK_0 0X40
    // 设置时钟中断的频率
    // 默认以最低的频率调用，这就够用了
    #define LATCH 0xffff
    out_p(PORT_CLOCK_0, LATCH & 0xff);
    out_p(PORT_CLOCK_0, LATCH >> 8);

    set_8259a_idt(INT_TIMER, timer_int);
    i8259A_irq_real(INT_TIMER);
}
