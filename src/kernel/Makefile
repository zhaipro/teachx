
all.o: kernel.o memory.o i8259.o sched.o ipc.o shell.o
	$(LD) $(LDFLAGS) -o $@ $^

include Makefile.d

Makefile.d: *.c
	$(CC) -MM *.c > Makefile.d

clean:
	rm -f *.o *.d
