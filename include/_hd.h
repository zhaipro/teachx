
#ifndef _HD_H_
#define _HD_H_
#include "type.h"

// 返回消息句柄 
int _hd_read(void *buf,u32 sector,u8 nsect);
int _hd_write(const void *buf,u32 sector,u8 nsect);
// 返回返回值 
bool_t hd_read(void *buf,u32 sector,u8 nsect);
bool_t hd_write(const void *buf,u32 sector,u8 nsect);

#endif
