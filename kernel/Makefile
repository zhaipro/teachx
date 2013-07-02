
NASM = D:\nasm\nasm.exe
NASMFLAGS = -f coff
CC = gcc
CFLAGS = -I../include -fno-builtin

all : asm.o kernel_asm.o vga_asm.o trap_asm.o keyboard_asm.o process_asm.o \
	_process_asm.o kernel.o _process.o errno.o vga.o trap.o keyboard.o \
	intc.o sched.o process.o clock.o ipc_asm.o

asm.o : asm.asm
	$(NASM) $(NASMFLAGS) -o asm.o asm.asm

kernel_asm.o : kernel.asm
	$(NASM) $(NASMFLAGS) -o kernel_asm.o kernel.asm

vga_asm.o : vga.asm
	$(NASM) $(NASMFLAGS) -o vga_asm.o vga.asm

trap_asm.o : trap.asm
	$(NASM) $(NASMFLAGS) -o trap_asm.o trap.asm

keyboard_asm.o : keyboard.asm
	$(NASM) $(NASMFLAGS) -o keyboard_asm.o keyboard.asm

process_asm.o : process.asm
	$(NASM) $(NASMFLAGS) -o process_asm.o process.asm

_process_asm.o : _process.asm
	$(NASM) $(NASMFLAGS) -o _process_asm.o _process.asm

ipc_asm.o : ipc.asm
	$(NASM) $(NASMFLAGS) -o ipc_asm.o ipc.asm
