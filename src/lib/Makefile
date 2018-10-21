
ASMFLAGS = -f coff

all: asm.o stdio.o

asm.o: asm.asm
	$(ASM) $(ASMFLAGS) -o asm.o asm.asm

include Makefile.d

Makefile.d: *.c
	$(CC) -M *.c > Makefile.d

clean:
	rm -f *.o *.d
