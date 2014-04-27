
CINC = ../include
CFLAGS = $(_CFLAGS) -I$(CINC)

OBJS = assert.o stdio.o string.o stdlib.o

clib_all.o : $(OBJS)
	$(LD) $(LDFLAGS) -o clib_all.o $(OBJS)

include Makefile.d

dep Makefile.d:
	gcc -M -I../include *.c > Makefile.d

clean :
	rm -f *.o Makefile.d
