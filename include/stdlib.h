
#ifndef _STDLIB_H_
#define _STDLIB_H_

typedef unsigned int size_t;

void* malloc(size_t size);
void free(void *p);
void realloc(void *p,size_t size);
char *itoa( int value, char *string, int radix );

#endif
