_hd.o: _hd.c ../../include/_process.h ../../include/type.h \
 ../../include/_sys_call.h ../../include/teachx.h ../../include/time.h
blk_sched.o: blk_sched.c ../../include/blk.h ../../include/type.h \
 ../../include/process.h ../../include/ipc.h ../../include/_process.h \
 ../../include/mm.h ../../include/vasm.h ../../include/_sys_call.h
floppy.o: floppy.c ../../include/asm.h ../../include/i386.h \
 ../../include/type.h ../../include/kernel.h ../../include/intc.h \
 ../../include/kernel.h
hd.o: hd.c ../../include/intc.h ../../include/kernel.h \
 ../../include/i386.h ../../include/type.h ../../include/setup.h \
 ../../include/asm.h ../../include/vga.h ../../include/process.h \
 ../../include/ipc.h ../../include/_process.h ../../include/mm.h \
 ../../include/vasm.h ../../include/_sys_call.h
lift.o: lift.c
