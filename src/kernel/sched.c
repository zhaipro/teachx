
#include <stddef.h>
#include "asm.h"
#include "i8259.h"
#include "ipc.h"
#include "kernel.h"
#include "memory.h"
#include "stdio.h"
#include "vga.h"


struct process_t {
    struct pushad_t pushad;
    interrupt_frame frame;  // 硬件层域
    uint32_t cr3;           // 虚拟内存
    struct process_t *next;
};

static struct process_t *s_free_procs = NULL;   // 空闲的存储列表
static struct process_t *s_cur_proc = NULL;     // 当前正在执行的

static void switch_to(struct process_t *next)
{
    // 在切换进程之前，保存当前进程的进度
    void *p = (void*)TOP_STACK - sizeof(interrupt_frame);
    s_cur_proc->frame = *(interrupt_frame*)p;
    *(interrupt_frame*)p = next->frame;
    p -= sizeof(struct pushad_t);
    s_cur_proc->pushad = *(struct pushad_t*)p;
    *(struct pushad_t*)p = next->pushad;
    // 切换到下一个进程
    s_cur_proc = next;
    lcr3(next->cr3);
}

void do_timer_int()
{
    static int s_time = 0;
    s_time ++;
    if (s_time % 1000 == 0)
        switch_to(s_cur_proc->next);
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

    extern void timer_int();
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

static void init_process(void (*start)())
{
    int i;
    s_free_procs = kmalloc();
    for(i = 1; i < _4K / sizeof(struct process_t); i++) {
        s_free_procs[i - 1].next = &s_free_procs[i];
    }
    s_free_procs[i - 1].next = NULL;
    s_cur_proc = s_free_procs;
    s_free_procs = s_free_procs->next;
    s_cur_proc->next = s_cur_proc;

    // 初始化寄存器数据
    s_cur_proc->frame.eip = (uint32_t)start;
    s_cur_proc->frame.cs = 3 * 8 + 1;
    s_cur_proc->frame.eflags = seflags();
    // 让内核进程也可以调用 I/O
    s_cur_proc->frame.eflags |= 0x1000;
    // 开启中断
    s_cur_proc->frame.eflags |= 0x200;
    s_cur_proc->frame.esp = 2 * _4M;
    s_cur_proc->frame.ss = 4 * 8 + 1;

    s_cur_proc->cr3 = scr3();
}

void do_fork()
{
    struct process_t *new_proc = s_free_procs;
    s_free_procs = s_free_procs->next;

    new_proc->next = s_cur_proc->next;
    s_cur_proc->next = new_proc;

    new_proc->frame = *(interrupt_frame*)(TOP_STACK - sizeof(interrupt_frame));
    new_proc->pushad = *(struct pushad_t*)(TOP_STACK - sizeof(interrupt_frame) - sizeof(struct pushad_t));
    new_proc->pushad.eax = 0;   // 新进程的返回值为0
    // 很无奈，我们下次fork内存
    new_proc->cr3 = s_cur_proc->cr3;
    memcpy(2 * _4M - 2 * _4K, 2 * _4M - _4K, _4K);
    new_proc->pushad.ebp -= _4K;
    new_proc->frame.esp -= _4K;
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

void init_sched(void (*start)())
{
    init_timer();
    init_desc();
    init_process(start);
    init_tss();
}

void run()
{
    lxs(4 * 8 + 1);
    iret(&s_cur_proc->frame);
}
