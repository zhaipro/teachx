
#ifndef _I8259_H_
#define _I8259_H_

// 8259A 控制器端口
// M => Master
// S => Slave
#define INT_M_CTRL      0X20
#define INT_M_CTRLMASK  0X21
#define INT_S_CTRL      0Xa0
#define INT_S_CTRLMASK  0Xa1

// 开启某中断
#define i8259A_irq_real(irq) do{    \
    if(irq<8)                       \
        out(INT_M_CTRLMASK, in(INT_M_CTRLMASK) & (uint8_t)(~(1<<irq)));         \
    else                                                                        \
        out(INT_S_CTRLMASK, in(INT_S_CTRLMASK) & (uint8_t)(~((1<<(irq))>>8)));  \
}while(0)

// 手动通知结束中断
#define eoi_m() out(INT_M_CTRL, 0X20)

// 中断号
#define INT_TIMER 0

#endif
