
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#define _4K (4u << 10)
#define _1M (1u << 20)
#define _4M (4u << 20)
#define _3G (3u << 30)

// 内核所在段
#define KERNEL_SEGMENT _3G
#define OFFSET_KERNEL (KERNEL_SEGMENT + 0x80600)
// 内核之前的 4M 内存用于存放页目录表(Page Directory Table)
#define PDT (KERNEL_SEGMENT - _4M)

#endif
