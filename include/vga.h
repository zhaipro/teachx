
#ifndef _VGA_H_
#define _VGA_H_
#include "type.h"

//用作vga_write_color的color参数 
#define FG_BLUE  0X0100
#define FG_GREEN 0X0200
#define FG_RED   0X0400
#define FG_WHITE (FG_BLUE | FG_GREEN | FG_RED)
#define FG_LIGHT 0X0800

#define BG_BLUE  0X1000
#define BG_GREEN 0X2000
#define BG_RED   0X4000
#define BG_WHITE (BG_BLUE | BG_GREEN | BG_RED)
#define BG_FLASH 0X8000

/* vga.asm */
u16* vga_write(u16 *addr,const char *str);
u16* vga_write_color(u16 *addr,const char *str,int color);

/*vga.c*/
void vga_set_cursor_pos(u32 pos);
void vga_set_start_addr(u32 addr);

void console_write(const char *str);

#endif
