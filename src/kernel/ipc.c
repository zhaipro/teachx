
#include "asm.h"
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
    printk("eax/edx/ecx: %d/%d/%d\n", param->eax, param->edx, param->ecx);
}

void init_ipc()
{
    set_user_gate(0x80, ipc);
    printk("Hello ipc!\n");
}
