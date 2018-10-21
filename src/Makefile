
export ASM = nasm
#                       获取绝对路径
export C_INCLUDE_PATH = $(shell readlink -f ./include)
export CC = gcc
export LDFLAGS = -r -i
export CFLAGS = -O

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
	$(LD) -o kernel.o -Ttext 0xc0080600 --section-alignment 4 $^
	objcopy -O binary -R .note -R .comment kernel.o $@

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
