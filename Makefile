
export LD = ld
export LDFLAGS = -r -i
export NASM = D:\nasm\nasm.exe
export CC = gcc
export _CFLAGS = -fno-builtin -Wimplicit-function-declaration
CINC = include
CFLAGS = $(_CFLAGS) -I$(CINC)

OBJS = kernel\kernel_all.o mm\mm_all.o clib\clib_all.o

all : tinix.img

tinix.img : setup.bin boot\boot.bin
	combine -c tinix.img -af boot\boot.bin
	tool\fat12\fat12.exe

setup.bin : kernel.bin boot\setup32.bin boot\setup16.bin
	@combine -n setup.bin -af boot\setup16.bin -s 512 -af boot\setup32.bin -s 2048 -af kernel.bin

boot\boot.bin boot\setup32.bin boot\setup16.bin : FORCE
	(cd boot; make)

kernel.bin : build_all.o
	@coff -Ttext 0x80509000 -o kernel.bin build_all.o

build_all.o : $(OBJS)
	$(LD) $(LDFLAGS) -o build_all.o $(OBJS)

kernel\kernel_all.o : FORCE
	(cd kernel; make)

mm\mm_all.o : FORCE
	(cd mm; make)

clib\clib_all.o : FORCE
	(cd clib; make)

FORCE :

dep :
	(cd boot; make dep)
	(cd kernel; make dep)
	(cd mm; make dep)
	(cd clib; make dep)

clean :
	rm -f *.bin *.o
	(cd boot; make clean)
	(cd kernel; make clean)
	(cd mm; make clean)
	(cd clib; make clean)
