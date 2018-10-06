
all: boot.bin

boot.bin: boot.asm utils.asm
	$(ASM) $< -o $@

clean:
	rm -f *.bin
