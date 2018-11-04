
#include "asm.h"
#include "kernel.h"
#include "stdio.h"

// Inter-Process Communication
__attribute__((interrupt))
static void ipc(void *_)
{
    printk("Hello ipc!\n");
}

void init_ipc()
{
    set_user_gate(0x80, ipc);
}
