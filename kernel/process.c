
/*
	目前只考虑单核处理器的实现 
*/
#include "asm.h"
#include "assert.h"
#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "sched.h"
#include "string.h"
#include "stdio.h"
#include "type.h" 
#include "intc.h"
#include "_sys_call.h"


static struct thread_t *tcbn;	// 指向线性组织的线程控制块表 
static struct process_t *pcbn;	// 指向线性组织的进程控制块表 
static struct thread_t *s_free_threads= NULL;	//空闲的PCB
static struct process_t *s_free_procs	= NULL;
static struct thread_t *s_cur_thread	= NULL;	//当前线程 
#define s_cur_process	(s_cur_thread->process)
static struct thread_t s_dead;					//僵死态，该线程已经结束，但是还有指向它的句柄存在。 
static struct thread_t s_hang;					//挂起态 
static struct TSS tss = {0};	//会用过的 

struct process_t* get_cur_proc()
{
	return s_cur_process;
}

struct thread_t* get_cur_thread()
{
	return s_cur_thread;
}

struct process_t* get_proc(int pid)
{
	return &pcbn[pid];
} 

void to_block_ex(struct thread_t *thread)
{
	if(thread->block_count == 0)
		sched_erase(thread);
	thread->block_count ++;
}

void to_block(int tid)
{
	to_block_ex(&tcbn[tid]);
}

void to_ready_ex(struct thread_t *thread)
{
	thread->block_count --;
	if(thread->block_count == 0)
		sched_insert(thread);
}

void to_ready(int tid)
{
	to_ready_ex(&tcbn[tid]);
}

void switch_to(struct thread_t *thread)
{
	//定义在process.asm中 
	extern void restart(struct cpu_context_t* context);
	
	assert(thread->block_count == 0);
	
	assert(thread->process && s_cur_process);
	if(thread->process->cr3 != s_cur_process->cr3){
		lcr3(thread->process->cr3);
	}

	s_cur_thread = thread;
	tss.esp0 = (u32)(&(s_cur_thread->context) + 1);
	restart(&s_cur_thread->context);
}

void set_retval(int tid,int retval)
{
	tcbn[tid].context.pushad.eax = retval;
}

void set_retval_ex(struct thread_t *thread,int retval)
{
	thread->context.pushad.eax = retval;
}

struct process_t* alloc_pcb()
{
	struct process_t *new_proc;
	
	if(s_free_procs == NULL){
		int i;
		new_proc = kvirtual_alloc(NULL,_4K);
		s_free_procs = new_proc;
		for(i=0;i<_4K/sizeof(struct process_t) - 1;i++)
		{
			new_proc[i].sibling = &new_proc[i+1];
		}
		new_proc[i].sibling = NULL;
	}
	
	new_proc = s_free_procs;
	s_free_procs = s_free_procs->sibling;
	
	return new_proc;
}

void free_pcb(struct process_t *pcb)
{
	pcb->sibling = s_free_procs;
	s_free_procs = pcb;
}

struct thread_t* alloc_tcb()
{
	struct thread_t *new_thread;
	
	if(s_free_threads == NULL){
		int i;
		new_thread = kvirtual_alloc(NULL,_4K);
		s_free_threads = new_thread;
		for(i=0;i<_4K/sizeof(struct thread_t) - 1;i++)
		{
			new_thread[i].sibling = &new_thread[i+1];
		}
		new_thread[i].sibling = NULL;
	}
	
	new_thread = s_free_threads;
	s_free_threads = s_free_threads->sibling;
	
	return new_thread;
}

void free_tcb(struct thread_t *tcb)
{
	tcb->sibling = s_free_threads;
	s_free_threads = tcb;
}

uint get_cur_pid()
{
	return s_cur_process - pcbn;
}

uint get_cur_tid()
{
	return s_cur_thread - tcbn;
}

// 创建进程控制块模板 
static struct process_t* create_proc()
{
/*
struct process_t{
	void *tls_free;
	void* pls[MAX_PLS];
};
*/
static bool_t created = FALSE;	// 记录是否已调用过本函数 
	struct thread_t *new_thread = alloc_tcb();
	struct process_t *new_proc = alloc_pcb();

// 初始化进程数据
	if(created == FALSE){
		new_proc->cr3 = scr3();
		created = TRUE;
	}else{
		new_proc->cr3 = create_vas();
	}

	new_proc->threads = new_thread;
	new_proc->mm.region_hdr = create_vasr(NULL,(void*)_4K,(void*)KERNEL_SEG);
	new_proc->child = NULL;
	new_proc->sibling = NULL;	// 由克隆函数填写，再此只是初始化 
	
	new_thread->status = STATUS_THREAD_HANG;
	new_thread->sched.life = MAX_THREAD_LIFE;
	new_thread->block_count = 1;
	new_thread->process = new_proc;
	new_thread->sibling = NULL;
	new_thread->recv_next = NULL;
	
	return new_proc;
}

void create_sys_proc(void (*proc_addr)())
{
	struct process_t *proc = create_proc();
	
	// 初始化主线程数据 
	// 初始化寄存器数据（不初始化通用寄存器） 
	proc->threads->context.ds = SELECTOR_SYS_DS;
	proc->threads->context.fs = SELECTOR_DUMMY;
	proc->threads->context.eip = (u32)proc_addr;
	proc->threads->context.cs = SELECTOR_SYS_CS;
	proc->threads->context.eflags = sflags();
	proc->threads->context.esp = SYS_STACK_TOP;
	proc->threads->context.ss = SELECTOR_SYS_DS;
	
	alloc_region(proc->mm.region_hdr,
		(void*)SYS_STACK_BASE,
		SYS_STACK_TOP-SYS_STACK_BASE,
		FLAG_PAGE_USER|FLAG_PAGE_WRITE|ATTR_PAGE_GNL);
	
	proc->threads->block_count = 0;
	s_cur_thread = proc->threads;
	sched_init(proc->threads);
}

// 设置TSS相关的内容 
void init_tss()
{
	set_gdt(INDEX_TSS_IN_GDT,&tss,sizeof(struct TSS),DA_386TSS|DA_DPL0);
	
	tss.backlink = 0;
	tss.ss0 = SELECTOR_KERNEL_DS;
	tss.esp0 = STACK_TOP;
	tss.iobase = sizeof(g_tss);
	ltr(SELECTOR_TSS);
}

void init_process_ctrl()
{
	s_dead.prev = s_dead.next = &s_dead;
	s_hang.prev = s_hang.next = &s_hang;
	
	init_tss();
	init_ipc();
	
	extern void clock_int();
	set_8259a_idt(INT_CLOCK,clock_int);
	enable_irq(INT_CLOCK);
}

static int do_fork(struct thread_t *p_thread)
{
	struct process_t *c_proc = create_proc();
	
	// 确立父子关系 
	c_proc->sibling = p_thread->process->child;
	p_thread->process->child = c_proc;

	// 克隆数据 
	c_proc->cr3 = p_thread->process->cr3;
	c_proc->threads->context = p_thread->context;

	// 设置子进程中的主线程的返回值 
	c_proc->threads->context.pushad.eax = 0;

// 子进程没有实际上等待的消息，因此直接将子进程放入就绪排队
// 其父进程则必须使用for_wait_msg唤醒它 
	to_ready(c_proc->threads - tcbn);
	
	return c_proc - pcbn;
}

void pm_process()
{
	int hmsg;
	int ret_val;
	struct msg_t msg;
	while(1)
	{
		hmsg = ipc_recv(&msg);
		switch(msg.type)
		{
		case SC_FORK:
//			ret_val = do_fork();
		break;
		default:
		
		break;
		}
		if(INVALID_HMSG != hmsg)
			ipc_for_wait(hmsg,ret_val);
	}
}
