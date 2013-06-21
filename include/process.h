
#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "_process.h"

int ipc_int(uint func,uint pid_to_or_hmsg,struct msg_t *pmsg);	// 中断函数 

int send_msg(uint pid_to,struct msg_t *pmsg,bool_t need_wait);
void for_wait_msg(int hmsg,int retval);
int wait_msg(int hmsg,bool_t need_retval);
int recv_msg(struct msg_t *pmsg);

uint get_cur_pid();
void* get_proc_regions(uint pid);
void set_proc_regions(uint pid,void *regions);

#define MAX_PSD	10
#define MAX_PLS	MAX_PSD
uint pls_alloc();
void pls_free(uint index);
void* pls_get_value(uint index);
void pls_set_value(uint index,void *p);

void schedule();

#define SYS_STACK_TOP	_2M
#define SYS_STACK_BASE	_1M

#endif
