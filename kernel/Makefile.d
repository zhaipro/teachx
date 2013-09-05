clock.o: clock.c ../include/_process.h ../include/type.h \
 ../include/_sys_call.h ../include/asm.h ../include/i386.h \
 ../include/config.h ../include/intc.h ../include/kernel.h \
 ../include/teachx.h ../include/time.h ../include/type.h \
 ../include/time.h ../include/assert.h
errno.o: errno.c ../include/type.h
handle.o: handle.c ../include/mm.h ../include/type.h ../include/kernel.h \
 ../include/i386.h ../include/handle.h
intc.o: intc.c ../include/asm.h ../include/i386.h ../include/type.h \
 ../include/intc.h ../include/kernel.h
ipc.o: ipc.c ../include/_sys_call.h ../include/ipc.h \
 ../include/_process.h ../include/type.h ../include/process.h \
 ../include/ipc.h ../include/mm.h ../include/vasm.h ../include/kernel.h \
 ../include/i386.h ../include/assert.h
kernel.o: kernel.c ../include/asm.h ../include/i386.h ../include/type.h \
 ../include/init.h ../include/kernel.h ../include/process.h \
 ../include/ipc.h ../include/_process.h ../include/mm.h ../include/vasm.h \
 ../include/sched.h ../include/process.h ../include/setup.h \
 ../include/stdio.h ../include/teachx.h ../include/time.h
keyboard.o: keyboard.c ../include/asm.h ../include/i386.h \
 ../include/type.h ../include/intc.h ../include/kernel.h \
 ../include/kernel.h ../include/keyboard.h ../include/vga.h
mktime.o: mktime.c ../include/time.h
process.o: process.c ../include/asm.h ../include/i386.h ../include/type.h \
 ../include/assert.h ../include/config.h ../include/kernel.h \
 ../include/mm.h ../include/process.h ../include/ipc.h \
 ../include/_process.h ../include/mm.h ../include/vasm.h \
 ../include/sched.h ../include/process.h ../include/string.h \
 ../include/stdio.h ../include/teachx.h ../include/time.h \
 ../include/type.h ../include/intc.h ../include/kernel.h ../include/ipc.h \
 ../include/_sys_call.h
sched.o: sched.c ../include/sched.h ../include/process.h ../include/ipc.h \
 ../include/_process.h ../include/type.h ../include/mm.h \
 ../include/vasm.h ../include/assert.h
teachx.o: teachx.c ../include/teachx.h ../include/type.h \
 ../include/time.h ../include/_process.h ../include/_sys_call.h
trap.o: trap.c ../include/kernel.h ../include/i386.h ../include/type.h \
 ../include/stdio.h
vga.o: vga.c ../include/asm.h ../include/i386.h ../include/type.h \
 ../include/type.h ../include/vga.h
