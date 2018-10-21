
#include <string.h>

#include "stdio.h"
#include "vga.h"

static char *itoa(int value, char *string)
{
    const int radix = 10;
    char *first = string;
    do{
        *string = value % radix + '0';
        value /= radix;
        string++;
    }while(value);
    *string = '\0';
    // 生成的数字是逆序的，所以要回转
    string--;
    for (char temp; string > first; first++, string--) {
        temp = *first;
        *first = *string;
        *string = temp;
    }
    return string;
}

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
        case 'd':
            itoa(*args, str);
            str += strlen(str);
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
