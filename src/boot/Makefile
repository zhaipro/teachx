
all: boot.bin setup.bin

boot.bin: boot.asm utils.asm include/load.inc
	$(ASM) $< -o $@

setup.bin: setup.asm.bin setup.c.bin
	cp setup.asm.bin setup.bin
	dd if=setup.c.bin of=$@ bs=512 seek=1 conv=notrunc

setup.asm.bin: setup.asm utils.asm include/load.inc
	$(ASM) $< -o $@

setup.c.bin: setup.o ../vga/vga.o
	$(LD) $(LDFLAGS) -o setup.all.o $^
	#                                   越小越好, 还有其它方法吗?
	$(LD) -o setup.tmp.o -Ttext 0x80200 --section-alignment 1 setup.all.o
	objcopy -O binary -R .note -R .comment setup.tmp.o $@

../vga/vga.o: FORCE
	cd ../vga && $(MAKE)

include Makefile.d

Makefile.d: *.c
	$(CC) -M *.c > Makefile.d

FORCE:

clean:
	rm -f *.bin *.o *.d
