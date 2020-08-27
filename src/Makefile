
export ASM = nasm
#                       获取绝对路径
export C_INCLUDE_PATH = $(shell readlink -f ./include)
export CC = gcc
export LDFLAGS = -r -i -m i386pe
export CFLAGS = -O -mgeneral-regs-only -m32
export OBJ2BIN = $(shell readlink -f obj2bin)

all: teachx.img

teachx.img: os.bin
	dd if=/dev/zero of=$@ bs=1440K count=1
	dd if=$< of=$@ conv=notrunc

os.bin: boot/boot.bin boot/setup.bin kernel.bin
	cat boot/boot.bin boot/setup.bin > $@
	dd if=kernel.bin of=$@ conv=notrunc bs=1 seek=2048

boot/boot.bin: FORCE
	cd boot && $(MAKE)

OBJS = kernel/all.o lib/all.o vga/vga.o

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.o $^
	$(OBJ2BIN) kernel.o $@ 0x080600

kernel/all.o: FORCE
	cd kernel && $(MAKE)

lib/asm.o: FORCE
	cd lib && $(MAKE)

vga/vga.o: FORCE
	cd vga && $(MAKE)

run: all
	bochs -q

debug: all
	bochsdbg -q

remake: clean all

rerun: clean run

FORCE:

clean:
	rm -f teachx.img *.bin *.o
	cd boot && $(MAKE) clean
	cd vga && $(MAKE) clean
	cd lib && $(MAKE) clean
	cd kernel && $(MAKE) clean
