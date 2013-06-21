
#include "stdlib.h"
#include "string.h"
#include "vga.h"

typedef int *va_list;

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;
	char	tmp[256];

	for (p=buf;*fmt;fmt++)
	{
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt)
		{
		case 'c':
			*p = *args;
			p++;
		break;
		case 'd':
			itoa(*args,p,10);
			p += strlen(p);
		break;
		case 's':
			strcpy(p,(const char*)(*args));
			p += strlen(p);
			break;
		case 'x':
			itoa(*args,p,16);
			p += strlen(p);
			break;
		default:
			break;
		}
		args++;
	}
	
	*p = '\0';
	return (p - buf);
}

int printf(const char *fmt, ...)
{
	char buf[256];
	int cnt;
	
	va_list arg = (va_list)(&fmt) + 1;
	cnt = vsprintf(buf, fmt, arg);
	console_write(buf);
	return cnt;
}
