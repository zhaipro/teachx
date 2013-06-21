
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "type.h"

#define KB_IN_BYTES 128

typedef struct {
	char *p_head;
	char *p_tail;
	char buf[KB_IN_BYTES];
}keyboard_buf;

#endif
