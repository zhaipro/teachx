
#ifndef _IO_H_ 
#define _IO_H_
#include "type.h"

// IO请求的当前状态 
#define REQ_TYPE_REQ	0	// 请求阶段 
#define REQ_TYPE_WAIT	1	// 等待阶段 
#define REQ_TYPE_COM	2	// 完成阶段 

// 用于 lseek函数的第三个参数 
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int close(handle_t h_file);
int eof(handle_t h_file);
long filelength(handle_t h_file);
long lseek(handle_t h_file,long offset,int origin);
int read(handle_t h_file,void *buf,size_t count);
long tell(handle_t h_file);
int write(handle_t h_file,const void *buf,size_t count);

#endif
