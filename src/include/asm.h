
#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>

#include "cpu.h"

typedef uint16_t port_t;

uint8_t in(port_t port);
void out(port_t port, uint8_t value);
void hlt();
void lcr3(uint32_t page_directory);
uint32_t scr0();
void lcr0(uint32_t cr0);
void jmp(uint32_t addr);
void lidt(struct idtptr_t *idtptr);
void sgdt(struct gdtptr_t *gdtptr);
uint32_t scr2();
void nop();
// 开启中断
void sti();
uint32_t seflags();
void iret(void *esp);
void lxs(uint32_t xs);

// 延迟输出
#define out_p(port, value) do{  \
    out(port, value); nop();    \
}while(0)

#endif
