
#ifndef _IPC_H_
#define _IPC_H_
#include "_process.h" 

struct _msg_t{
	int tid_wait;			// Ö»ÔÚ
	uint flag;
	int retval;
	struct msg_t msg;
	struct _msg_t *next;
};

#endif
