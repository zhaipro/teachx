
export ASM = nasm

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

FORCE:

clean:
	rm -f teachx.img *.bin
	cd boot && $(MAKE) clean
