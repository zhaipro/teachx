
#include "asm.h"
#include "i8259.h"
#include "kernel.h"
#include "stdio.h"

// KBD_US
// ASCII \033 == Esc
static char s_key_map[127] =
    "\000\033"
    "1234567890-="
    "\b\t"
    "qwertyuiop[]"
    "\n\000"
    "asdfghjkl;'"
    "`\000"
    "\\zxcvbnm,./"
    "\000*\000 "
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "-\000\000\000+";

static char s_shift_map[127] =
    "\000\033"
    "!@#$%^&*()_+"
    "\b\t"
    "QWERTYUIOP{}"
    "\n\000"
    "ASDFGHJKL:\""
    "~\000"
    "|ZXCVBNM<>?"
    "\000*\000 "
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "-\000\000\000+";

#define isbreak(scan) ((scan) & 0x80)
#define ismake(scan)  (!isbreak(scan))

__attribute__((interrupt))
static void int_keyboard(void *_)
{
    static int scan_code = 0;           // 扫描码
    static int e0 = 0;
    #define IN_BUFFER 0x60
    scan_code <<= 8;
    scan_code |= in(IN_BUFFER);
    if (e0) {
        e0--;
    } else if (scan_code == 0xe0) {
        e0 = 1;                         // 表示其后还跟随着1个字符扫描码
    } else if (scan_code == 0xe1) {
        e0 = 2;
    }
    if (e0 == 0) {
        printk("sc: %X, ismake: %X\n", scan_code, ismake(scan_code));
        scan_code = 0;
    }
    eoi_m();
}

void init_keyboard()
{
    #define INT_KEYBOARD 1
    set_8259a_idt(INT_KEYBOARD, int_keyboard);
    i8259A_irq_real(INT_KEYBOARD);
    printk("Init kerboard ok!\n");
}
