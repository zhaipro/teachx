
#include <stddef.h>

#include "asm.h"
#include "ipc.h"
#include "kernel.h"
#include "memory.h"
#include "sched.h"
#include "stdio.h"

struct _msg_t{
    struct msg_t msg;
    struct _msg_t *next;
};

static struct _msg_t *s_msgs;
static struct _msg_t *s_free_msgs;
static struct _msg_t *s_cur_msgs = NULL;

static struct _msg_t* alloc_msg_block()
{
    struct _msg_t *new_block = s_free_msgs;
    s_free_msgs = s_free_msgs->next;
    return new_block;
}

static void send(int32_t pid, struct msg_t *msg)
{
    struct _msg_t *_msg = alloc_msg_block();
    _msg->msg = *msg;
}

static void recv(int32_t pid)
{
    struct _msg_t *_msg = s_cur_msgs;
    for(; _msg; _msg=_msg->next)
    {

    }
}

// Inter-Process Communication
__attribute__((regparm(3)))
int32_t do_ipc(int32_t eax, int32_t edx, int32_t ecx)
{
    struct _msg_t *_msg = NULL;
    if(eax >= 0) {      // 发送给指定进程
        if(edx | IPC_SEND) {    // 发送
            send(eax, (struct msg_t*)ecx);
        } else {                // 接收
            recv(eax);
        }
        _msg = alloc_msg_block();
        _msg->msg = *((struct msg_t*)ecx);
        printk("eax: %d, msg.ps: %d/%d/%d\n", eax, _msg->msg.p0, _msg->msg.p1, _msg->msg.p2);
        return 0;
    }
    // 小于零表示消息发送给内核
    return do_fork();
}

void init_ipc()
{
    int i;
    s_free_msgs = (struct _msg_t*)kmalloc();
    for(i=0; i<_4K/sizeof(struct _msg_t)-1; i++)
    {
        s_free_msgs[i].next = &s_free_msgs[i + 1];
    }
    s_free_msgs[i].next = NULL;

    extern void ipc();
    set_user_gate(0x80, ipc);
    printk("Hello ipc!\n");
    printk("Max count of message: %d\n", _4K / sizeof(struct _msg_t));
}
