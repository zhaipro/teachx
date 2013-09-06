
# 这里make依赖kernel下的asm.o，因此上层makefile必须先make kernel

CINC = ../include
CFLAGS = $(_CFLAGS) -I$(CINC)

all : setup32.bin boot.bin setup16.bin

include Makefile.d

setup32.bin : setup32_asm.o setup32.o ../kernel/asm.o
	coff -Ttext 0x80300 -o setup32.bin setup32_asm.o setup32.o ..\kernel\asm.o

boot.bin : boot.asm
	$(NASM) boot.asm -o boot.bin

setup16.bin : setup16.asm
	$(NASM) setup16.asm -o setup16.bin

setup32_asm.o : setup32.asm
	$(NASM) -f coff -o setup32_asm.o setup32.asm

dep :
	gcc -M -I../include *.c > Makefile.d

clean :
	rm -f *.o *.bin
