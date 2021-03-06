
#ifndef _IPC_H_
#define _IPC_H_
#include "_process.h" 

struct _msg_t{
	int tid_wait;			// 只在
	uint flag;
	int retval;
	struct msg_t msg;
	struct _msg_t *next;
};

void init_ipc();
// 为中断例程提供的接口
// 之，发送一条消息，且不等待返回 
void _send_msg(uint pid,struct msg_t*);

// 之，发送一条消息，并等待，但不带回返回值 
void _send_wait_msg(uint pid,struct msg_t*);

// 无需中断的ipc函数，即它们不会造成线程堵塞 
void for_wait_msg(int mid,int retval); 

#endif
