
#ifndef __PROCESS_H_
#define __PROCESS_H_
#include "type.h"

union p_t{
	int iparam;
	uint uiparam;
	long lparam;
	unsigned long ulparam;
	void *pparam;
	const void *cpparam;
};

// ipc 机制中的消息 
struct msg_t{
	uint pid_from;		// 由系统自动填充 
	uint type;
	union p_t p0;
	union p_t p1;
	union p_t p2;
};

#define INVALID_HMSG	-1
int ipc_recv(struct msg_t *pmsg);
int ipc_send(uint pid,struct msg_t *pmsg,bool_t for_wait);
int ipc_wait(uint hmsg);
int ipc_send_wait(uint pid,struct msg_t *pmsg);
void ipc_for_wait(uint hmsg,int retval);

#define MAX_TLS	(_4K/sizeof(void*))
#define MAX_TSD	MAX_TLS

typedef uint pthread_key_t;

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
void* pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
int fork();

#endif
