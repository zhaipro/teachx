
all: boot.bin setup.bin

boot.bin: boot.asm utils.asm include/load.inc
	$(ASM) $< -o $@

setup.bin: setup.asm.bin setup.c.bin
	cp setup.asm.bin setup.bin
	dd if=setup.c.bin of=$@ bs=512 seek=1 conv=notrunc

setup.asm.bin: setup.asm utils.asm include/load.inc
	$(ASM) $< -o $@

setup.c.bin: setup.o ../vga/vga.o ../lib/asm.o
	$(LD) $(LDFLAGS) -o setup.all.o $^
	$(OBJ2BIN) setup.all.o $@ 0x80200

../vga/vga.o: FORCE
	cd ../vga && $(MAKE)

../lib/asm.o: FORCE
	cd ../lib && $(MAKE)

include Makefile.d

Makefile.d: *.c
	$(CC) -MM *.c > Makefile.d

FORCE:

clean:
	rm -f *.bin *.o *.d
