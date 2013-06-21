
#include "init.h"
#include "kernel.h"
#include "asm.h"
#include "stdio.h"
#include "setup.h"

struct TSS g_tss = {0};	//会用过的 

// 设置TSS相关的内容 
static void init_tss()
{
	set_gdt(INDEX_TSS_IN_GDT,&g_tss,sizeof(struct TSS),DA_386TSS|DA_DPL0);
	
	g_tss.backlink = 0;
	g_tss.ss0 = SELECTOR_KERNEL_DS;
	g_tss.esp0 = STACK_TOP;
	g_tss.iobase = sizeof(g_tss);
	ltr(SELECTOR_TSS);
}

// 功能：
// 保证任何时候系统中都至少有一个非堵塞进程 
// 让CPU休息休息 
static void sys_idle_proc()
{
	while(1)
		hlt();
}

static void init_proc()
{
	create_sys_proc(mm_process);
	create_sys_proc(hd_process);
	create_sys_proc(sys_idle_proc);
}

void kernel_start()
{
	struct setup_info_t system_info = *g_sys_info;
	
	init_tss();
	
	init_trap();
	init_8259A();
	sti();
	//	printf("first:%x,last:%x\n",g_sys_info->first_page_addr,g_sys_info->last_page_addr);
	init_mm(g_sys_info->first_page_addr,g_sys_info->last_page_addr);
	init_keyboard();
	init_process_ctrl();
	init_proc();
	schedule();
}
