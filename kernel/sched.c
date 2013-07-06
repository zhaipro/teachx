
#include "sched.h"
#include "assert.h"

// 将可调度线程组织成双指针域的循环链表 
// 其中 idle_head 为表头，且指向空闲进程的线程 
static struct thread_t *idle_head;
static struct thread_t *next_thread;


static void schedule()
{
	next_thread = next_thread->sched.next;
	if(next_thread == idle_head)
		next_thread = next_thread->sched.next;
}

void sched_init(struct thread_t *idle_thread)
{
	idle_head = idle_thread;
	next_thread = idle_thread;
	idle_head->sched.prev = idle_head;
	idle_head->sched.next = idle_head;
	idle_head->sched.life = 0;
} 

void sched_insert(struct thread_t *thread)
{
	thread->sched.prev = idle_head->sched.prev;
	thread->sched.next = idle_head;
	idle_head->sched.prev->sched.next = thread;
	idle_head->sched.prev = thread;
	thread->sched.life = MAX_THREAD_LIFE;
}

void sched_erase(struct thread_t *thread)
{
	// 调度函数需要 next_thread ，因此必须在删除该节点之前调用。 
	if(next_thread == thread){
		schedule();
		if(next_thread == thread)
			next_thread = idle_head;
	}
	
	thread->sched.prev->sched.next = thread->sched.next;
	thread->sched.next->sched.prev = thread->sched.prev;
}

struct thread_t* do_iret()
{	
	if(next_thread->sched.life <= 0){
		next_thread->sched.life += MAX_THREAD_LIFE;
		schedule();
	}
	
	return next_thread;
}

void do_sched_clock_int()
{
	next_thread->sched.life --;
}
