
NASMFLAGS = -f coff
CINC = ../../include
CFLAGS = $(_CFLAGS) -I$(CINC)

OBJS = blk_sched.o _hd.o hd.o fdhd_asm.o

blk_drv_all.o : $(OBJS)
	$(LD) $(LDFLAGS) -o blk_drv_all.o $(OBJS)

fdhd_asm.o : fdhd.asm
	$(NASM) $(NASMFLAGS) -o fdhd_asm.o fdhd.asm

include Makefile.d

dep :
	gcc -M -I../../include *.c > Makefile.d

clean :
	rm -f *.o
