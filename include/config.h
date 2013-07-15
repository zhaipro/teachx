
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define _1K (1u<<10)
#define _2K (1u<<11)
#define _4K (1u<<12)
#define _1M (1u<<20)
#define _2M (1u<<21)
#define _4M (1u<<22)

// 系统中允许的进程/线程的最大个数 
#define MAX_PROC_COUNT		50
#define MAX_THREAD_COUNT	100

#define KERNEL_SEG		(1u<<31)				// 2G
#define PTT 			KERNEL_SEG
#define PDT				(PTT + (PTT>>10))
#define VGA				(PTT + _4M)
#define IDT				(VGA + 8*_4K)
#define GDT				(IDT + _2K)

#define STACK_BOTTOM	(GDT + _2K)
#define STACK_TOP		(STACK_BOTTOM + _1M)
#define kernel_entry	STACK_TOP				// 0x509000
#define KERBEL_END		(KERNEL_SEG + 16*_1M)
#define PTE_BEGIN		KERBEL_END
#define PTE_END			(KERNEL_SEG + 1024*_1M)
#define VIEW_BUF_BEGIN	(KERNEL_SEG + 512*_1M)	// 视图缓冲区位置 
#define VIEW_BUF_END	PTE_END

#endif
