
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

// 内核会在调用该模块其它函数前调用该初始化函数，你可以随便做点什么。 
void init_vasm();

// 创建一个内存区域管理器，并指定被管理区域大小[end,end2) 
// 当然，[begin,end)不可用 
region_hdr_t create_vasr(void *begin,void *end,void *end2);

// 复制一份管理器 
region_hdr_t vasm_do_fork(region_hdr_t hdr);

// 从指定区域链中分配一个区域 
void* alloc_region(region_hdr_t hdr,void *addr,size_t size,uint flag);

// 暂时没有实现 
bool_t reset_region(region_hdr_t hdr,const void *addr,uint flag);

// 释放一个区域 
// 如果指定地址不属于任何区域则返回false 
bool_t free_region(region_hdr_t hdr,void *addr);

// 得到指定地址所在区域的标志
// 如果addr不在任何区域中，则返回0 
uint region_flag(region_hdr_t hdr,const void *addr);

#endif
