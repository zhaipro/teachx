
OBJS = vga.o

all: $(OBJS)

include Makefile.d

Makefile.d: *.c
	$(CC) -M *.c > Makefile.d

clean:
	rm -f *.o *.d
