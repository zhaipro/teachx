
#ifndef _SCHED_H_
#define _SCHED_H_
#include "type.h"

// 使指定线程可被调度 
void sched_insert(int tid);
// 使指定线程不可被调度 
void sched_erase(int tid);
// 当中断返回到进程时被调用，嵌套的中断返回不会调用该函数 
void do_iret();

#endif
