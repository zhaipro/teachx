
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

os.bin: boot/boot.bin boot/setup.bin
	cat $^ > $@

boot/boot.bin: FORCE
	cd boot && $(MAKE)

run: all
	bochs -q

debug: all
	bochsdbg -q

remake: clean all

rerun: clean run

FORCE:

clean:
	rm -f teachx.img *.bin
	cd boot && $(MAKE) clean
	cd vga && $(MAKE) clean
	cd lib && $(MAKE) clean
