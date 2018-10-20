
all: kernel.o

include Makefile.d

Makefile.d: *.c
	$(CC) -M *.c > Makefile.d

clean:
	rm -f *.o *.d
