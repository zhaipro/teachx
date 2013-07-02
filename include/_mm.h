
#ifndef __MM_H_
#define __MM_H_
#include "type.h"

// 没有定义 FLAG_PAGE_READ ，这是因为这会让用户以为，如果他们不设置该标志，那该区段就不可读。
// 然而这是错误的认为。 
#define FLAG_PAGE_WRITE		0X02
#define FLAG_PAGE_USER		0X04
#define ATTR_PAGE_MASK		0XF000
#define ATTR_PAGE_UNKNOW	0X0000		// 未知 
#define ATTR_PAGE_GNL		0X1000		// 提交普通页面 
#define ATTR_PAGE_ZERO		0x2000		// 提交被轻清零的页面 
#define ATTR_PAGE_FMAP		0x3000		// 文件映射 

void* virtual_alloc(void *addr,size_t size,uint flag);
bool_t virtual_reset(void *addr,uint flag);
bool_t virtual_free(void *addr);
bool_t mapview_file(void *addr,handle_t handle,int offset);

#endif
