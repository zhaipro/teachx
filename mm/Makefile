
NASM = D:\nasm\nasm.exe
NASMFLAGS = -I ../include/ -f coff
CC = gcc
CFLAGS = -I../include -fno-builtin

all : mm_asm.o mm.o vasm.o

mm_asm.o : mm.asm
	$(NASM) $(NASMFLAGS) -o mm_asm.o mm.asm
