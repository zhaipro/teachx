
ASMFLAGS = -f coff

all.o: kernel.o memory.o i8259.o sched.o ipc.o shell.o kernel.asm.o
	$(LD) $(LDFLAGS) -o $@ $^

kernel.asm.o: kernel.asm
	$(ASM) $(ASMFLAGS) -o $@ $^

include Makefile.d

Makefile.d: *.c
	$(CC) -MM *.c > Makefile.d

clean:
	rm -f *.o *.d
