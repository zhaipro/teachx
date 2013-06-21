
#include "stdio.h"
#include "stdlib.h"

void _assert (const char* e, const char* filename, int line)
{
	printf("Assertion failed: %s, file %s, line %d\n",e,filename,line);
	exit(-1);
}
