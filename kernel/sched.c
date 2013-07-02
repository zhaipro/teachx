
#include "sched.h"
#include "intc.h"
#include "assert.h"
#include "stdio.h"

// 将可调度线程组织成双指针域的循环链表 
// 其中 idle_head 为表头，且指向空闲进程的线程 
static struct thread_t *idle_head;

void sched_init(struct thread_t *idle_thread)
{
	idle_head = idle_thread;
	idle_head->sched.prev = idle_head;
	idle_head->sched.next = idle_head;
} 

void sched_insert(struct thread_t *thread)
{
	thread->sched.prev = idle_head->sched.prev;
	thread->sched.next = idle_head;
	idle_head->sched.prev->sched.next = thread;
	idle_head->sched.prev = thread;
}

void sched_erase(struct thread_t *thread)
{
	thread->sched.prev->sched.next = thread->sched.next;
	thread->sched.next->sched.prev = thread->sched.prev;
}

struct thread_t* schedule()
{
	struct thread_t *new_thread;
	
	new_thread = idle_head->sched.next;
	
	return new_thread;
}

struct thread_t* do_iret()
{
	struct thread_t *cur_thread = get_cur_thread();
	struct thread_t *new_thread;

	assert(cur_thread);
	
	if(cur_thread->sched.life <= 0){
		cur_thread->sched.life += MAX_THREAD_LIFE;
		sched_erase(cur_thread);
		sched_insert(cur_thread);
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
