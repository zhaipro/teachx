buffer.o: buffer.c ../include/kernel.h ../include/i386.h \
 ../include/type.h
mm.o: mm.c ../include/mm.h ../include/type.h ../include/_sys_call.h \
 ../include/asm.h ../include/i386.h ../include/kernel.h \
 ../include/process.h ../include/ipc.h ../include/_process.h \
 ../include/mm.h ../include/vasm.h ../include/stdio.h ../include/string.h \
 ../include/teachx.h ../include/time.h ../include/type.h \
 ../include/assert.h
swap.o: swap.c
vasm.o: vasm.c ../include/vasm.h ../include/type.h ../include/mm.h
