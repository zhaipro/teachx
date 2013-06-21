
#ifndef _INIT_H_
#define _INIT_H_
#include "type.h"

void init_kernel();

void init_trap();
void init_8259A();
void init_keyboard();
void init_mm(u32 first_page,u32 last_page);
void init_process_ctrl();
void hd_process();
void mm_process();

#endif
