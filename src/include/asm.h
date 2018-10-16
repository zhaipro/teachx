
#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>

typedef uint16_t port_t;

void out(port_t port, uint8_t value);
void hlt();
void lcr3(uint32_t page_directory);
uint32_t scr0();
void lcr0(uint32_t cr0);

#endif
