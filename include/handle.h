
#ifndef _HANDLE_H_
#define _HANDLE_H_
#include "type.h"

typedef (*fp_close_handle_t)(void*);

void init_handle_table();

bool_t close_handle(handle_t handle); 
void* get_knl_obj(handle_t handle,fp_close_handle_t close_proc);
handle_t open_handle(void *obj,fp_close_handle_t close_proc);

#endif
