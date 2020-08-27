
ASMFLAGS = -f coff

all.o: asm.o stdio.o bios.o
	$(LD) $(LDFLAGS) -o $@ $^

asm.o: asm.asm
	$(ASM) $(ASMFLAGS) -o asm.o asm.asm

include Makefile.d

Makefile.d: *.c
	$(CC) -MM *.c > Makefile.d

clean:
	rm -f *.o *.d
