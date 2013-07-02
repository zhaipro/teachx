
#ifndef _HANDLE_H_
#define _HANDLE_H_
#include "type.h"

typedef (*fp_close_handle_t)(void*);

void init_handle_table();

handle_t open_handle(int pid,void *obj,fp_close_handle_t close_proc);
void* get_knl_obj(int pid,handle_t handle,fp_close_handle_t close_proc);
void close_handle(int pid,handle_t handle);
void close_all_handle(int pid);

#endif
