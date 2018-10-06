
all: boot.bin setup.bin


boot.bin: boot.asm utils.asm include/load.inc
	$(ASM) $< -o $@

setup.bin: setup.asm utils.asm include/load.inc
	$(ASM) $< -o $@

clean:
	rm -f *.bin
