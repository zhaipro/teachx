
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "ipc.h"
#include "mm.h"
#include "vasm.h"
#include "_process.h"

// pushad will push
struct pushad_t{
	u32	edi;		// ↑ Low
	u32	esi;
	u32	ebp;
	u32	esp;		// <- 'popad' will ignore it
	u32	ebx;	
	u32	edx;
	u32	ecx;
	u32	eax;		// ┷Hig
};

/*
在用户态，中断发生将依次压栈：ss,esp,eflags,cs,eip。
在内核态（中断态），中断发生将依次压栈：eflags,cs,eip。 
...注意：在保护模式下（即BITS 32），pop push iret等都默认为32位指令。 
用户进程只是用cs、ds段寄存器，gs为全局段寄存器 
*/
struct cpu_context_t{	//		↑ Low
	u32	ds_es;
	u32 fs;
	struct pushad_t pushad;
	u32 return_addr;
	u32 error_code;
	u32	eip;			// ┓
	u32	cs;				// ┃
	u32	eflags;			// ┣ these are pushed by CPU during interrupt
	u32	esp;			// ┃
	u32	ss;				// ┛	┷High
};

struct thread_t;
struct process_t{
	u32 cr3;					// 进程的页目录表的物理地址
	struct thread_t *threads;	// 进程中的线程
	struct process_t *child;	// 指向第一个子进程 
	struct process_t *sibling;	// 指向兄弟进程 

// ipc module data
	struct{
		struct _msg_t *msg_tail;// 该进程的待处理消息排队 
		struct thread_t *recv_tail;	
								// 用于链接该进程中因recv而堵塞的线程们 
	}ipc;

// mm module data
	struct{
		region_hdr_t region_hdr;
	}mm;
	
	#define MAX_PSD	10
	#define MAX_PLS	MAX_PSD
	void *tls_free;
	void* pls[MAX_PLS];
};

struct thread_t{

// 硬件层域 
	struct cpu_context_t context;
	int block_count;			// 记录该线程被多少次要求堵塞 
	struct process_t *process;	// 指向自身线程所属的进程
	struct thread_t *sibling;	// 指向自己的兄弟线程，用于链接该进程中的所有线程。 
	
// sched module data
	struct{
		#define MAX_THREAD_LIFE		20
		int life;
		struct thread_t *prev;
		struct thread_t *next; 
	}sched;

// ipc module data
	struct{
		struct thread_t *next;	// 用于等待消息的线程排队 
	}ipc;
#define STATUS_THREAD_FREE	0
#define STATUS_THREAD_READY	1
#define STATUS_THREAD_RECV	2	// 因接收消息而进入堵塞
#define STATUS_THREAD_WAIT	3	// 因等待返回而进入堵塞 
#define STATUS_THREAD_DEAD	4
#define STATUS_THREAD_HANG	8	// 这是一个掩码位，可在任意状态下进入挂起态 
	uint status;

	u32 exit_code;
	
	struct thread_t *recv_next;	// 用于链接该进程中因recv而堵塞的线程们 
	struct thread_t *prev;
	struct thread_t *next;
};

void to_block(int tid);
void to_block_ex(struct thread_t*);
void to_ready(int tid);
void to_ready_ex(struct thread_t*);
void set_retval(int tid,int retval);
void set_retval_ex(struct thread_t*,int retval);

uint get_cur_pid();
uint get_cur_tid();
struct process_t* get_cur_proc();
struct thread_t* get_cur_thread();
struct process_t* get_proc(int pid);

uint pls_alloc();
void pls_free(uint index);
void* pls_get_value(uint index);
void pls_set_value(uint index,void *p);
struct thread_t* create_sys_proc(void (*proc_addr)());

#define SYS_STACK_TOP	_2M
#define SYS_STACK_BASE	_1M

#endif
