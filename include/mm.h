
#ifndef _MM_H_
#define _MM_H_
/*
页表的4M空间全部可以使用
如果用户正确使用，则设置好然后使用，
如果用户错误访问内存，则使用没有设置好的表项，最终导致错误。
文件（物理内存）内核对象可以共享，但是映射对象则不可以，
因为每个程序可能将相同的文件（物理内存）映射到不同的虚拟内存地址中。 

一个区域的大小必须大于零 

文件映射机制： 
打开文件
映射文件 创建映射内核对象 
映射视图 直接映射到内存就可以了，即使其指向映射对象。 
*/
#include "type.h"

#define MEM_MASH			0XE00
#define MEM_COMMIT			0X200
#define MEM_COMMIT_RESET	0X400
#define MEM_NULL			0X600

void init_user_space(u32);
cr3_t create_vas();

#endif
