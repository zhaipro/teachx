
ASMFLAGS = -f coff

all: asm.o

asm.o: asm.asm
	$(ASM) $(ASMFLAGS) -o asm.o asm.asm

clean:
	rm -f *.o
