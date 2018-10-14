
#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>

typedef uint16_t port_t;

void out(port_t port, uint8_t value);

#endif
