
export ASM = nasm

all: teachx.img

teachx.img: boot/boot.bin
	dd if=/dev/zero of=$@ bs=1440K count=1
	dd if=$< of=$@ conv=notrunc

boot/boot.bin: FORCE
	cd boot && $(MAKE)

run: all
	bochs -q

debug: all
	bochsdbg -q

FORCE:

clean:
	rm -f teachx.img
	cd boot && $(MAKE) clean
