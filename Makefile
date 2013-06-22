
CC = gcc
CFLAGS = -Iinclude -Iclib/inc

objects = kernel\kernel_asm.o kernel\kernel.o \
	kernel\init.o mm\mm.o mm\mm_asm.o kernel\intc.o \
	kernel\keyboard.o kernel\keyboard_asm.o kernel\trap.o kernel\trap_asm.o \
	kernel\vga.o kernel\vga_asm.o kernel\process.o kernel\process_asm.o \
	kernel\block_drv\hd.o kernel\block_drv\fdhd.o kernel\asm.o clib\scr\assert.o \
	clib\scr\stdio.o clib\scr\string.o clib\scr\stdlib.o

all : setup\setup32.bin setup.bin

kernel.bin : $(objects)
	@coff -Ttext 0x80509000 -o kernel.bin $(objects)

tinix.img : setup\boot.bin
	@combine -c tinix.img -af setup\boot.bin
setup.bin : setup\setup16.bin setup\setup32.bin kernel.bin
	@combine -n setup.bin -af setup\setup16.bin -s 512 -af setup\setup32.bin -s 1536 -af kernel.bin
