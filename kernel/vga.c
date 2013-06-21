
#include "type.h"
#include "vga.h"

#define CTRLC_ADDR_PORT	0X3D4
#define CTRLC_DATA_PORT	0X3D5
#define CTRLC_INDEX_START_ADDR_H 0XC
#define CTRLC_INDEX_START_ADDR_L 0XD
#define CTRLC_INDEX_CURSOR_H 0XE
#define CTRLC_INDEX_CURSOR_L 0XF

void vga_set_cursor_pos(u32 pos)
{
	out(CTRLC_ADDR_PORT, CTRLC_INDEX_CURSOR_H);
	out(CTRLC_DATA_PORT, pos>>8);
	out(CTRLC_ADDR_PORT, CTRLC_INDEX_CURSOR_L);
	out(CTRLC_DATA_PORT, pos & 0xff);
}

void vga_set_start_addr(u32 addr)
{
	out(CTRLC_ADDR_PORT, CTRLC_INDEX_START_ADDR_H);
	out(CTRLC_DATA_PORT, addr>>8);
	out(CTRLC_ADDR_PORT, CTRLC_INDEX_START_ADDR_L);
	out(CTRLC_DATA_PORT, addr & 0xff);
}

void console_write(const char *str)
{
static u16 *s_addr = 0;	//视频缓冲区段，默认起始地址是0 
	s_addr = vga_write_color(s_addr,str,FG_GREEN);
	vga_set_cursor_pos((u32)s_addr>>1);
}
