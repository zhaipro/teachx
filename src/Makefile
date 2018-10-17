
export ASM = nasm
export CC = gcc -std=c99

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
	which find
	find . \( -name *.bin \)
	find . \( -name *.o \)
	find . \( -name *.bin -o -name *.img -o -name *.o -o -name *.d \) -print0 | xargs -0 rm -f
	find . \( -name *.bin \)
#	find . \( -name *.img -o -name *.bin -o -name *.o -o -name *.d \) -print0 | xargs -0 rm -f
