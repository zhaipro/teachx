
CINC = ../include
CFLAGS = $(_CFLAGS) -I$(CINC)

OBJS = mm_asm.o mm.o vasm.o

mm_all.o : $(OBJS)
	$(LD) $(LDFLAGS) -o mm_all.o $(OBJS)

mm_asm.o : mm.asm
	$(NASM) -f coff -o mm_asm.o mm.asm

include Makefile.d

dep :
	gcc -M -I../include *.c > Makefile.d

clean :
	rm -f *.o
