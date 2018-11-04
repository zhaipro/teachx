
#include "asm.h"
#include "ipc.h"
#include "kernel.h"
#include "stdio.h"

struct regparm_t {
    uint32_t eax;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebp;
};

// Inter-Process Communication
__attribute__((interrupt))
static void ipc(void *frame)
{
    struct regparm_t *param = ((struct regparm_t*)frame) - 1;
    struct msg_t *msg = (struct msg_t*)param->ecx;
    printk("msg.ps: %d/%d/%d\n", msg->p0, msg->p1, msg->p2);
}

void init_ipc()
{
    set_user_gate(0x80, ipc);
    printk("Hello ipc!\n");
}
