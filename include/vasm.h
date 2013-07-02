
#ifndef _VASRM_H_
#define _VASRM_H_
// virtual address space region management
/*
该模块仅仅管理区域的分配、释放和标志，但并不对标志进行解释。 
该模块必须拥有一些静态变量，用于最初的create_vasr(...);调用。 
之后空间不足时可以调用kvirtual_alloc(...);来获取空间，该函数定义在mm模块中。 
*/

#include "type.h"

// 使用与malloc同样的思想管理虚拟内存 
// 已使用块[begin,end)
// 空闲块  [end,end2) 
struct region_t{
	u32 flag;
	void *begin;
	void *end;
	void *end2; 

	struct region_t *prev;
	struct region_t *next;
};
typedef struct region_t* region_hdr_t;

void init_vasm();
region_hdr_t create_vasr(void *begin,void *end,void *end2);
region_hdr_t vasm_do_fork(region_hdr_t hdr);
void* alloc_region(region_hdr_t hdr,void *addr,size_t size,uint flag);
bool_t reset_region(region_hdr_t hdr,const void *addr,uint flag);
bool_t free_region(region_hdr_t hdr,void *addr);
uint region_flag(region_hdr_t hdr,const void *addr);

#endif
