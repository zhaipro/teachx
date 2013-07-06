setup32.o: boot/setup32.c include/kernel.h include/i386.h include/type.h \
 include/asm.h \
 d:\codeblocks\mingw\bin\../lib/gcc/mingw32/4.4.1/../../../../include/memory.h \
 include/string.h include/setup.h
_process.o: kernel/_process.c include/_process.h include/type.h \
 include/_sys_call.h
clock.o: kernel/clock.c
errno.o: kernel/errno.c include/type.h
handle.o: kernel/handle.c include/mm.h include/_mm.h include/type.h \
 include/kernel.h include/i386.h include/handle.h
intc.o: kernel/intc.c include/asm.h include/i386.h include/type.h \
 include/intc.h include/kernel.h
ipc.o: kernel/ipc.c include/ipc.h include/_process.h include/type.h \
 include/process.h include/ipc.h include/mm.h include/_mm.h \
 include/vasm.h include/kernel.h include/i386.h include/assert.h
kernel.o: kernel/kernel.c include/asm.h include/i386.h include/type.h \
 include/init.h include/kernel.h include/process.h include/ipc.h \
 include/_process.h include/mm.h include/_mm.h include/vasm.h \
 include/sched.h include/process.h include/setup.h include/stdio.h
keyboard.o: kernel/keyboard.c include/asm.h include/i386.h include/type.h \
 include/intc.h include/kernel.h include/kernel.h include/keyboard.h \
 include/vga.h
process.o: kernel/process.c include/asm.h include/i386.h include/type.h \
 include/assert.h include/config.h include/kernel.h include/mm.h \
 include/_mm.h include/process.h include/ipc.h include/_process.h \
 include/mm.h include/vasm.h include/sched.h include/process.h \
 include/string.h include/stdio.h include/type.h include/intc.h \
 include/kernel.h include/ipc.h include/_sys_call.h
sched.o: kernel/sched.c include/sched.h include/process.h include/ipc.h \
 include/_process.h include/type.h include/mm.h include/_mm.h \
 include/vasm.h include/assert.h
trap.o: kernel/trap.c include/kernel.h include/i386.h include/type.h \
 include/stdio.h
vga.o: kernel/vga.c include/asm.h include/i386.h include/type.h \
 include/type.h include/vga.h
assert.o: clib/assert.c include/stdio.h include/stdlib.h include/type.h
ctype.o: clib/ctype.c
io.o: clib/io.c
main.o: clib/main.c include/stdio.h include/string.h include/type.h \
 include/stdlib.h
stdio.o: clib/stdio.c include/stdio.h include/stdlib.h include/type.h \
 include/string.h include/vga.h
stdlib.o: clib/stdlib.c include/stdlib.h include/type.h
string.o: clib/string.c include/string.h include/type.h
buffer.o: mm/buffer.c include/kernel.h include/i386.h include/type.h
mm.o: mm/mm.c include/type.h include/kernel.h include/i386.h \
 include/type.h include/mm.h include/_mm.h include/process.h \
 include/ipc.h include/_process.h include/mm.h include/vasm.h \
 include/asm.h include/stdio.h include/string.h include/assert.h \
 include/_sys_call.h
swap.o: mm/swap.c
vasm.o: mm/vasm.c include/vasm.h include/type.h include/mm.h \
 include/_mm.h
_hd.o: kernel\blk_drv\_hd.c include/_hd.h include/type.h \
 include/_process.h include/_sys_call.h
blk_sched.o: kernel\blk_drv\blk_sched.c include/type.h include/_process.h \
 include/type.h include/_sys_call.h
floppy.o: kernel\blk_drv\floppy.c include/asm.h include/i386.h \
 include/type.h include/kernel.h include/intc.h include/kernel.h
hd.o: kernel\blk_drv\hd.c include/intc.h include/kernel.h include/i386.h \
 include/type.h include/setup.h include/asm.h include/vga.h \
 include/process.h include/ipc.h include/_process.h include/mm.h \
 include/_mm.h include/vasm.h include/_sys_call.h
