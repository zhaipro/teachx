
#ifndef _MEMORY_H_
#define _MEMORY_H_

void init_memory();
uint32_t create_pd();
void* kmalloc();
uint32_t do_memory_fork();

#endif
