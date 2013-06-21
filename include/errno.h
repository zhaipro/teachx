
#ifndef _ERRNO_H_
#define _ERRNO_H_
#include "type.h"

void panic(u32 error_code,u32 param);

//返回一个与线程绑定的错误代码 
u32* _errno();
#define errno (*_errno())


/* 定义所有内核错误代码 */

//The operation completed successfully
#define ERROR_SUCCESS	0

//Unable to send byte to FDC
#define ERROR_FDC		1

//指定内存无法读取 
#define ERROR_MEM_READ	2

//指定内存无法写入 
#define ERROR_MEM_WRITE	3 

//没有足够的句柄空间了
#define	EMFILE		24	/* Too many open files */

//无效句柄值
#define	EBADF		9	/* Bad file descriptor */

#endif
