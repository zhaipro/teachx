
#include "process.h"
#include "sched.h"
#include "intc.h"
#include "asm.h"
#include "kernel.h"

/*
static u32 task_stack[STACK_SIZE_TOTAL];

void entry()
{
	s_proc.regs.cs		= (8 * 0) | SA_TIL | SA_RPL3;
	s_proc.regs.ds		= (8 * 1) | SA_TIL | SA_RPL3;
	s_proc.regs.ss		= (8 * 1) | SA_TIL | SA_RPL3;
	s_proc.regs.eip	= (u32)TestA;
	s_proc.regs.esp	= (u32) task_stack + STACK_SIZE_TOTAL;
	s_proc.regs.eflags	= 0x1202;	// IF=1, IOPL=1, bit 2 is always 1.
	
	restart(&s_proc);
}

	g_tss.ss0 = SELECTOR_KERNEL_DS;
	g_tss.esp0 = (u32)((&s_proc.regs) + 1);
	g_tss.iobase = sizeof(g_tss);

*/

struct thread_t* schedule()
{
	if(s_cur_thread){
		if(s_cur_thread->life<0){
			s_cur_thread->life += 20;
			to_ready(s_cur_thread);
			s_cur_thread = NULL;
		}else if(s_cur_thread->status != STATUS_THREAD_READY){
			s_cur_thread = NULL;
		}
	}
	
	if(s_cur_thread == NULL){
		if(s_ready.next != &s_ready){
			switch_to(s_ready.next);
		}else
			assert(!"no a process!");
	}
}

struct thread_t* do_iret()
{
	struct thread_t *cur_thread = get_cur_thread();
	struct thread_t *new_thread;

	if(cur_thread->sched.life <= 0){
		cur_thread->sched.life += 20;
		new_thread = schedule();
	}else{
		new_thread = cur_thread;
	}
	
	return new_thread;
}

void do_clock_int()
{
	struct thread_t *cur_thread = get_cur_thread();
	eoi_m();
	cur_thread->sched.life --;
}
