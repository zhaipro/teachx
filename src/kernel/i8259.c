
#include "asm.h"
#include "i8259.h"
#include "kernel.h"

// 中断控制器字
#define ICW1    0X11    // 上升沿触发，级联，要写入 ICW4
#define ICW2_M  INT_VECTOR_IRQ
#define ICW2_S  INT_VECTOR_IRQ + 8
#define ICW3_M  0X4     // IR2 为级联线
#define ICW3_S  0X2
#define ICW4    0X1     // 非自动结束，数据线不缓冲

void init_8259A()
{
    out(INT_M_CTRL, ICW1);
    out(INT_S_CTRL, ICW1);
    out(INT_M_CTRLMASK, ICW2_M);
    out(INT_S_CTRLMASK, ICW2_S);
    out(INT_M_CTRLMASK, ICW3_M);
    out(INT_S_CTRLMASK, ICW3_S);
    out(INT_M_CTRLMASK, ICW4);
    out(INT_S_CTRLMASK, ICW4);
    out(INT_M_CTRLMASK, 0Xff);  // 屏蔽所有中断
    out(INT_S_CTRLMASK, 0Xff);  // 稍后开启
}
