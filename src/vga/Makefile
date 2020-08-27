
OBJS = vga.o

all: $(OBJS)

include Makefile.d

Makefile.d: *.c
	$(CC) -MM *.c > Makefile.d

clean:
	rm -f *.o *.d
