
#include "asm.h"
#include "init.h"
#include "kernel.h"
#include "process.h"
#include "sched.h"
#include "setup.h"
#include "stdio.h"
#include "teachx.h"


// 功能：
// 保证任何时候系统中都至少有一个非堵塞进程 
// 让CPU休息休息 
static void sys_idle_proc()
{
	int i;
	sti();
	
	while(1)
	{
		// 暂时不允许所有进程都被堵塞。 
		for(i=0;i<10000000;i++)
			nop();
		printf("(1)");
	}
	
	while(1)
		hlt();
}

static void test_proc2()
{
	int i;
	void *buf;
	struct msg_t msg;
	int mid;
	int retval;
	
	sti();
	
	buf = kvirtual_alloc(NULL,_4K);
	hd_read_msg(buf,1,1);
	printf(buf);
	
	for(i=0;;i++)
	{
		printf("(2%d)",i);
		sleep(1 * CLOCKS_PER_SEC);
	}
}

static void test_proc3()
{
	int i;
	
	sti();
	
	for(i=0;;i++)
	{
		printf("(3%d)",i);
		sleep(0.5 * CLOCKS_PER_SEC);
	}
}

static void init_proc()
{
	struct thread_t *thread;
//	create_sys_proc(mm_process);
	thread = create_sys_proc(sys_idle_proc);
	sched_init(thread);
	thread = create_sys_proc(test_proc2);
	sched_insert(thread);
	thread = create_sys_proc(test_proc3);
	sched_insert(thread);
	thread = create_sys_proc(hd_process);
	sched_insert(thread);
	thread = create_sys_proc(clock_process);
	sched_insert(thread);
}

void kernel_start()
{
	struct setup_info_t system_info = *g_sys_info;
	u32 flags;

	init_trap();
	init_8259A();
	init_hd();
	init_mm(g_sys_info->first_page_addr,g_sys_info->last_page_addr);
	init_keyboard();
	init_clock();
	init_process_ctrl();
	
	flags = sflags();
	flags |= 0x1000;
	lflags(flags);
	
	init_proc();
	
	printf("\n");

	// 从此系统初始化完毕。 
}
