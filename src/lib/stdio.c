
#include "stdio.h"
#include "vga.h"

typedef int *va_list;

int vsprintf(char *buf, const char *fmt, va_list args)
{
    char *str;

    for (str=buf; *fmt; fmt++) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        ++fmt;

        switch (*fmt) {
        case 'c':
            *str = *args;
            str++;
            break;
        default:
            break;
        }
        args++;
    }

    *str = '\0';
    return str - buf;
}

int printk(const char *fmt, ...)
{
    char buf[128];
    int i;

    va_list args = (va_list)(&fmt) + 1;
    i = vsprintf(buf, fmt, args);
    vga_write(buf);
    return i;
}
