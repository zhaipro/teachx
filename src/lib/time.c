/*
这里的内容是从 Linux-0.11 的 ./init/main.c 中抄袭来的
当然，做了一点小小的改动
*/

#include <time.h>

#include "bios.h"
#include "stdio.h"

extern long kernel_mktime(struct tm * tm);

#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)

// Yeah, yeah, it's ugly, but I cannot find how to do this correctly and this seems to work.
//     -- Linus Torvalds
// 该方法丑陋的原因是其获取的是所在地的时间
// 因此无法准确地计算出时间戳
void time_init(void)
{
    struct tm time;

    do {
        time.tm_sec = cmos_read(0);
        time.tm_min = cmos_read(2);
        time.tm_hour = cmos_read(4);
        time.tm_mday = cmos_read(7);
        time.tm_mon = cmos_read(8);
        time.tm_year = cmos_read(9);
    } while (time.tm_sec != cmos_read(0));
    BCD_TO_BIN(time.tm_sec);
    BCD_TO_BIN(time.tm_min);
    BCD_TO_BIN(time.tm_hour);
    BCD_TO_BIN(time.tm_mday);
    BCD_TO_BIN(time.tm_mon);
    BCD_TO_BIN(time.tm_year);
    printk("%d-%d-%d %d:%d:%d\n", time.tm_year,
                                  time.tm_mon,
                                  time.tm_mday,
                                  time.tm_hour,
                                  time.tm_min,
                                  time.tm_sec);
}
