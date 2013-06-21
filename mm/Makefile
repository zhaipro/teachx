
NASM = @D:\\nasm\\nasm.exe
NASMFLAGS = -I ../include/ -f coff
CC = @gcc
CFLAGS = -I../include -I../clib/inc

all : mm_asm.o mm.o

mm_asm.o : mm.asm
	$(NASM) $(NASMFLAGS) -o mm_asm.o mm.asm
