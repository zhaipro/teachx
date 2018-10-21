
all.o: kernel.o memory.o
	$(LD) $(LDFLAGS) -o $@ $^

include Makefile.d

Makefile.d: *.c
	$(CC) -M *.c > Makefile.d

clean:
	rm -f *.o *.d
