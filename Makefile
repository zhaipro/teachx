
CC = gcc
CFLAGS = -Iinclude -fno-builtin -Wimplicit-function-declaration

objects = kernel\kernel_asm.o kernel\kernel.o \
	mm\mm.o mm\mm_asm.o mm\vasm.o kernel\intc.o \
	kernel\keyboard.o kernel\keyboard_asm.o kernel\trap.o kernel\trap_asm.o \
	kernel\vga.o kernel\vga_asm.o kernel\process.o kernel\process_asm.o \
	kernel\block_drv\hd.o kernel\block_drv\fdhd.o kernel\asm.o clib\assert.o \
	clib\stdio.o clib\string.o clib\stdlib.o kernel\ipc.o kernel\_process.o \
	kernel\_process_asm.o kernel\sched.o kernel\ipc_asm.o

all : boot\setup32.bin setup.bin tinix.img

kernel.bin : $(objects)
	@coff -Ttext 0x80509000 -o kernel.bin $(objects)

tinix.img : boot\boot.bin
	@combine -c tinix.img -af boot\boot.bin
setup.bin : boot\setup16.bin boot\setup32.bin kernel.bin
	@combine -n setup.bin -af boot\setup16.bin -s 512 -af boot\setup32.bin -s 2048 -af kernel.bin
