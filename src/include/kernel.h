
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "cpu.h"
#include "settings.h"

#define SELECTOR_KERNEL_CS 8

#define set_trap_gate(n, addr)  \
    set_gate(&((struct descriptor_t*)IDT)[n], DA_386IGate, addr, SELECTOR_KERNEL_CS)

// 8259a 中断向量起点
#define INT_VECTOR_IRQ 0x20
// 中断号
#define INT_CLOCK 0
#define set_8259a_idt(n, addr) set_trap_gate((n) + INT_VECTOR_IRQ, addr)

#endif
