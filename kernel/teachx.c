
#include "teachx.h"
#include "_process.h"
#include "_sys_call.h"


// 定义在 teachx.asm 中 
int ipc(uint func,uint pid_to_or_hmsg,struct msg_t *pmsg);

int ipc_recv(struct msg_t *pmsg)
{
	int mid = ipc(IPC_RECV,0,pmsg);
	if(mid == -2)
		mid = ipc(IPC_RECV,0,pmsg);
	return mid;
}

int ipc_send(uint pid,struct msg_t *pmsg,bool_t for_wait)
{
	if(for_wait == TRUE)
		return ipc(IPC_SEND | IPC_ATTR_WAIT,pid,pmsg);
	else
		return ipc(IPC_SEND,pid,pmsg);
}
 
int ipc_wait(uint hmsg)
{
	return ipc(IPC_WAIT,hmsg,NULL);
} 

int ipc_send_wait(uint pid,struct msg_t *pmsg)
{
	return ipc(IPC_SEND_WAIT,pid,pmsg);
}

void ipc_for_wait(uint hmsg,int retval)
{
	struct msg_t msg;
	msg.p0.iparam = retval;
	ipc(IPC_FOR_WAIT,hmsg,&msg);
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
	struct msg_t msg;
	msg.type = SC_PTHREAD_KEY_CREATE;
	*key = ipc_send_wait(PID_PM,&msg);
	return *key;
}

int pthread_key_delete(pthread_key_t key)
{
	struct msg_t msg;
	msg.type = SC_PTHREAD_KEY_DELETE;
	msg.p0.uiparam = key;
	return ipc_send_wait(PID_PM,&msg);
}

void* pthread_getspecific(pthread_key_t key)
{
extern void* _pthread_getspecific(pthread_key_t);

	return (key < MAX_TSD)? _pthread_getspecific(key):NULL;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
extern int _pthread_setspecific(pthread_key_t key, const void *value);

	if(key < MAX_TSD)
		_pthread_setspecific(key,value);
}

int fork()
{
	struct msg_t msg;
	msg.type = SC_FORK;
// 这里是一个BUG
// 这里的函数运行的用户态，也就是说我们无法直接调用内核函数：get_cur_tid
	msg.p0.uiparam = get_cur_tid();
	return ipc_send_wait(PID_PM,&msg);
}


clock_t clock()
{
	struct msg_t msg;
	msg.type = SC_CLK_CLOCK;
	return ipc_send_wait(PID_CLK,&msg);
}

void sleep(clock_t wait)
{
	struct msg_t msg;
	msg.type = SC_CLK_SLEEP;
	msg.p0.uiparam = wait;
	ipc_send_wait(PID_CLK,&msg);
}

time_t time(time_t *pt)
{
	struct msg_t msg;
	time_t t;
	msg.type = SC_CLK_TIME;
	t = ipc_send_wait(PID_CLK,&msg);
	if(pt)
		*pt = t;
	return t;
}
