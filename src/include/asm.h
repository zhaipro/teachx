
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
uint32_t scr2();

#endif
