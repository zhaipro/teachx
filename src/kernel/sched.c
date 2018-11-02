
#include "asm.h"
#include "i8259.h"
#include "kernel.h"
#include "stdio.h"
#include "vga.h"

__attribute__((interrupt)) static void timer_int(void *_)
{
    static int s_time = 0;
    s_time ++;
    if (s_time % 1000 == 0)
        printk("Hello timer! %d\n", s_time);
    eoi_m();
}

static void init_timer()
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

static void init_desc()
{
    struct gdtptr_t gdt;
    sgdt(&gdt);
    // 描述符类型值说明
    #define DA_32           0x400000    // 32 位段
    #define DA_LIMIT_4K     0x800000    // 段界限粒度为 4K 字节
    #define DA_DPL1         0x2000
    // 存储段描述符类型值说明
    #define DA_DRW          0x9200      // 存在的可读写数据段属性值
    #define DA_C            0x9800      // 存在的只执行代码段属性值
    // 内核进程段
    set_desc(&(gdt.addr[3]), 0, 0xfffff, DA_C | DA_32 | DA_LIMIT_4K | DA_DPL1);
    set_desc(&(gdt.addr[4]), 0, 0xfffff, DA_DRW | DA_32 | DA_LIMIT_4K | DA_DPL1);
}

static interrupt_frame s_proc;

static void create_process(void (*start)())
{
    // 初始化寄存器数据
    s_proc.eip = (uint32_t)start;
    s_proc.cs = 3 * 8 + 1;
    s_proc.eflags = seflags();
    // 让内核进程也可以调用 I/O
    s_proc.eflags |= 0x1000;
    // 开启中断
    s_proc.eflags |= 0x200;
    s_proc.esp = 0x08000;
    s_proc.ss = 4 * 8 + 1;
}

static void shell_process()
{
    printk("Shell process ...\n");
    while(1);
}

// 只使用 ss0 和 esp0
static struct {
    uint32_t back_link; // 16 high bits zero
    uint32_t esp0;      // stack pointer to use during interrupt
    uint32_t ss0;       // 16 high bits zero. segment
}s_tss;

static void init_tss()
{
    struct gdtptr_t gdt;
    sgdt(&gdt);
    // 存储段描述符类型值说明
    #define DA_386TSS 0x8900  // 可用 386 任务状态段类型值
    #define SELECTOR_TSS 5
    set_desc(&(gdt.addr[SELECTOR_TSS]), &s_tss, sizeof(s_tss), DA_386TSS);

    s_tss.back_link = 0;
    s_tss.ss0 = 16; // selector kernel ds
    s_tss.esp0 = TOP_STACK;
    ltr(SELECTOR_TSS * 8);
}

void init_sched()
{
    init_timer();
    init_desc();
    create_process(shell_process);
    init_tss();
}

void run()
{
    lxs(4 * 8 + 1);
    iret(&s_proc);
}
