
#include "sched.h"
#include "intc.h"
#include "asm.h"
#include "kernel.h"

#define NUM_PROCESS 1

TSS g_tss = {0};
PROCESS g_process[NUM_PROCESS];

/*
static u32 task_stack[STACK_SIZE_TOTAL];


static void delay()
{
	int i,j;
	for(i=0;i<100;i++)
	for(j=0;j<10000;j++)
	{
		
	}
}

static void TestA()
{
	int i=0;
	while(1)
	{
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay();
	}
}

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


void init_sched()
{
	set_tss(&g_tss);
	ltr(SELECTOR_TSS);
}

void do_clock_int(PROCESS *cur)
{
	eoi_m();
	switch_to(cur);
}
