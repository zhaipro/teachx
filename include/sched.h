
#ifndef _SCHED_H_
#define _SCHED_H_
/*
	这里放置了所有关于调度的代码，也就是说，改变这里就可以实现任何你希望实现的调
度策略。
	出于教学目的，这里也仅仅放置了关于调度的代码。
		函数 void do_clock_int(); 会被系统中的其它模块回调，因此该模块不必设置中
	断向量。
		中断返回时，函数 struct thread_t* do_iret(); 会被调用，需要调度模块返回
	一个线程，该线程会得到到CPU，至于如何得到CPU也无需该模块关心。 
*/
#include "process.h"

// 指定一个不会被堵塞、挂起的空闲线程 
void sched_init(struct thread_t *idle_thread);

// 使指定线程可被调度 
void sched_insert(struct thread_t*);

// 使指定线程不可被调度 
void sched_erase(struct thread_t*);

// 当中断返回到进程时被调用，嵌套的中断返回不会调用该函数 
struct thread_t* do_iret();

// 时钟中断会调用该函数 
void do_clock_int();

#endif
