
#ifndef _ASM_H_
#define _ASM_H_

#include "i386.h"

void cli();
void enable_pagine();
void hlt();
u8 in(port_t port);
void lcr2(u32 r2);
void lcr3(u32 cr3);
void lflags(u32 flags);
void lgdt(struct GDTPTR*);
void lidt(struct IDTPTR*);
void ltr(u32 selector);
void nop();
void out(port_t port,u8 value);
//nr位字数，而不是字节数 
void port_readw(port_t port,void *buf,u32 nr);
void port_writew(port_t port,void *buf,u32 nr);
u32 scr2();
u32 scr3();
u32 sflags();
void sti();
//延迟输入输出 
#define in_p(port)	({	\
	nop();in(port);		\
})

#define out_p(port,value) do{	\
	out(port,value);nop();		\
}while(0)


//void jmp(u32 selector,u32 offset); 

#endif
