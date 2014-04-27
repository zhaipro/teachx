
NASMFLAGS = -f coff -I../include/
CINC = ../include
CFLAGS = $(_CFLAGS) -I$(CINC)

# kernel_asm.o 必须排在前面！！！
OBJS = kernel_asm.o asm.o clock.o intc.o ipc.o kernel.o keyboard.o mktime.o process_asm.o \
	process.o sched.o teachx_asm.o teachx.o trap_asm.o trap.o vga_asm.o vga.o \
	blk_drv/blk_drv_all.o

kernel_all.o : $(OBJS)
	$(LD) $(LDFLAGS) -o kernel_all.o $(OBJS)

include Makefile.d

blk_drv/blk_drv_all.o :
	(cd blk_drv; make)

asm.o : asm.asm
	$(NASM) $(NASMFLAGS) -o asm.o asm.asm

kernel_asm.o : kernel.asm
	$(NASM) $(NASMFLAGS) -o kernel_asm.o kernel.asm

vga_asm.o : vga.asm
	$(NASM) $(NASMFLAGS) -o vga_asm.o vga.asm

trap_asm.o : trap.asm
	$(NASM) $(NASMFLAGS) -o trap_asm.o trap.asm

process_asm.o : process.asm
	$(NASM) $(NASMFLAGS) -o process_asm.o process.asm

teachx_asm.o : teachx.asm
	$(NASM) $(NASMFLAGS) -o teachx_asm.o teachx.asm

dep Makefile.d:
	# 在指定路径是必须使用/
	gcc -M -I../include *.c > Makefile.d
	(cd blk_drv;make dep)

clean :
	rm -f *.o Makefile.d
	(cd blk_drv; make clean)
