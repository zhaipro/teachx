
#include <stddef.h>

#include "asm.h"
#include "ipc.h"
#include "kernel.h"
#include "memory.h"
#include "stdio.h"

struct _msg_t{
    struct msg_t msg;
    struct _msg_t *next;
};

static struct _msg_t *s_free_msgs;

struct regparm_t {
    uint32_t eax;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebp;
};

static struct _msg_t* alloc_msg_block()
{
    struct _msg_t *new_block = s_free_msgs;
    s_free_msgs = s_free_msgs->next;
    return new_block;
}

// Inter-Process Communication
__attribute__((interrupt))
static void ipc(void *frame)
{
    struct regparm_t *param = ((struct regparm_t*)frame) - 1;
    struct _msg_t *_msg = alloc_msg_block();
    _msg->msg = *((struct msg_t*)param->ecx);
    printk("msg.ps: %d/%d/%d\n", _msg->msg.p0, _msg->msg.p1, _msg->msg.p2);
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

    set_user_gate(0x80, ipc);
    printk("Hello ipc!\n");
    printk("Max count of message: %d\n", _4K / sizeof(struct _msg_t));
}
