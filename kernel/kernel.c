
#include "asm.h"
#include "init.h"
#include "kernel.h"
#include "process.h"
#include "setup.h"
#include "stdio.h"


// 功能：
// 保证任何时候系统中都至少有一个非堵塞进程 
// 让CPU休息休息 
static void sys_idle_proc()
{
	int i,j;
	
	sti();
	
	for(i=0;;i++)
	{
		for(j=0;j<1000000;j++)
			nop();
		printf("(1%d)",i);
		for(j=0;j<1000000;j++)
			nop();
	}
	while(1)
		hlt();
}

static void test_proc2()
{
	int i,j;
	struct msg_t msg;
	int mid;
	int retval;
	
	sti();
	
	msg.type = 2013;
	mid = ipc_send(2,&msg,TRUE);
	retval = ipc_wait(mid);
	printf("retval = %d",retval);
	
	for(i=0;;i++)
	{
		for(j=0;j<500000;j++)
			nop();
		printf("(2%d)",i);
		for(j=0;j<500000;j++)
			nop();
	}
}

static void test_proc3()
{
	struct msg_t msg;
	int i,j;
	int mid;
	
	sti();
	
	mid = ipc_recv(&msg);
	
	printf("mid = %d msg.type = %d",mid,msg.type);
	
	for_wait_msg(mid,1990);
	
	for(i=0;;i++)
	{
		for(j=0;j<1000000;j++)
			nop();
		printf("(3%d)",i);
		for(j=0;j<1000000;j++)
			nop();
	}
}

static void init_proc()
{
	struct thread_t *thread;
//	create_sys_proc(mm_process);
//	create_sys_proc(hd_process);
	thread = create_sys_proc(sys_idle_proc);
	sched_init(thread);
	thread = create_sys_proc(test_proc2);
	sched_insert(thread);
	thread = create_sys_proc(test_proc3);
	sched_insert(thread);
}

void kernel_start()
{
	struct setup_info_t system_info = *g_sys_info;
	u32 flags;
	
	init_trap();
	init_8259A();
	//	printf("first:%x,last:%x\n",g_sys_info->first_page_addr,g_sys_info->last_page_addr);
	init_mm(g_sys_info->first_page_addr,g_sys_info->last_page_addr);
	printf("end of mm\nstart keyboard\n");
	init_keyboard();
	printf("end of keyboard\nstart process\n");
	init_process_ctrl();
	printf("end of process\nstart proc\n");
	
	flags = sflags();
	flags |= 0x1000;
	lflags(flags);
	
	init_proc();
	printf("end of proc\n");
	
	// 从此系统初始化完毕，时钟中断后即可进入系统进程。 
	sti();
}
