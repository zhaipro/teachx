#ifndef _VGA_H_
#define _VGA_H_

int vga_write(int addr, const char* str);
void vga_write_line(int line, const char* str);

#endif
