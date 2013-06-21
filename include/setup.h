
#ifndef _SETUP_H_
#define _SETUP_H_
#include "type.h"

struct setup_info_t{
	uint first_page_addr;
	uint last_page_addr;
	u8  hd0[16];			//int 0x41 0x104
	u8  hd1[16];			//int 0x46 0x118
	u8  fd_info[11];		//int 0x1e 0x78
	u8  cmos_data[0x34];
};

#define g_sys_info ((struct setup_info_t*)(0x400))

#endif
