
#ifndef _UNISTD_H_
#define _UNISTD_H_
#include "type.h"

/* lseek */
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

/* exit */
#define EXIT_GENERAL			-1
#define EXIT_NOT_ENOUGH_MEMORY	-2

int close (handle_t handle);
void exit(int status);
long lseek (handle_t handle, long offset, int origin);
handle_t open(const char *filename,int flag);
int read (handle_t handle, void* buf, size_t size);
long tell(handle_t handle);
int write (handle_t handle, const void* buf, size_t size);
int remove(const char *filename);
int rename (const char *oldname, const char *newname);

#endif
