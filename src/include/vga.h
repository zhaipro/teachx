#ifndef _VGA_H_
#define _VGA_H_

void vga_set_cursor_pos(int pos);
int vga_write(const char* str);
int _vga_write(int addr, const char* str);

#endif
