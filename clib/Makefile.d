assert.o: assert.c ../include/stdio.h ../include/stdlib.h \
 ../include/type.h
ctype.o: ctype.c
io.o: io.c
main.o: main.c ../include/stdio.h ../include/string.h ../include/type.h \
 ../include/stdlib.h
stdio.o: stdio.c ../include/stdio.h ../include/stdlib.h ../include/type.h \
 ../include/string.h ../include/vga.h
stdlib.o: stdlib.c ../include/stdlib.h ../include/type.h
string.o: string.c ../include/string.h ../include/type.h
