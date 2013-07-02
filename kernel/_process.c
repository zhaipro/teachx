
#include "_process.h"
#include "_sys_call.h"

/* 该值与内核中的定义必须一致，也就说这里有数据耦合。 */
#define IPC_RECV		0X01	// 接收消息
#define IPC_SEND		0X02	// 发送消息 
#define IPC_WAIT		0X03	// 等待
#define IPC_SEND_WAIT	0X04	// 发送消息，并等待该消息返回 
#define IPC_FOR_WAIT	0X05	// 

#define IPC_ATTR_WAIT	0X10	// 只可以与 IPC_WAIT 掩码 


// 定义在 _process.asm 中 
int ipc(uint func,uint pid_to_or_hmsg,struct msg_t *pmsg);

int ipc_recv(struct msg_t *pmsg)
{
	return ipc(IPC_RECV,0,pmsg);
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
	msg.p0.uiparam = get_cur_tid();
	return ipc_send_wait(PID_PM,&msg);
}
