
/*
在这里所有关于内存的数据都是以页为单位。 
vaddr 代表虚拟地址或线性地址
paddr 代表物理地址（页框地址） 
*/

#include "type.h"
#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "asm.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "_sys_call.h"

typedef u32 pte_t;

#define ptt			((pte_t*)PTT)
#define pdt			((pte_t*)PDT)

/* 硬件解释 */ 
#define PAGE_PRESENT		0X01		// 存在标志
#define PAGE_WRITE			0X02		// 可写标志
#define PAGE_USER			0X04		// 用户区
#define PAGE_ACCESSED		0X20		// 已被访问 
#define PAGE_DIRTY			0X40		// 已被修改 

/* 软件解释 之 此页不存在 */ 
#define ATTR_PAGE_UNKNOW	0X0000		// 未知 
#define ATTR_PAGE_GNL		0X1000		// 提交普通页面 
#define ATTR_PAGE_ZERO		0x2000		// 提交被轻清零的页面 
#define ATTR_PAGE_FMAP		0x3000		// 文件映射 

/* 软件解释 之 此页已存在
   只可以使用那三个处理器
   为用户保留的标志位     */
#define ATTR_PAGE_FWRITE	0X200		// 该页面为文件映射页面，且可写，但需要通知文件管理器 
#define ATTR_PAGE_FCOW		0X400		// 该页面为文件映射页面，但不可写，但可写时复制
#define ATTR_PAGE_PCOW		0X600		// 该页面来自于进程复制，需要写时复制 
#define ATTR_PAGE_COPYED	0X800		// 该页面也被写时复制过了，此时可写标志应已存在 


#define get_pte_data(pte)	((pte)>>12)
#define set_pte_data(pte,data)	do{pte=((pte)&0xfffff000)|(data);}while(0)

//物理内存的首、尾地址，4K字节对齐
struct PFN_t{
	union{
		struct PFN_t *next;		// 指向下一个同类型的物理页
								// 只有两种类型的页可以使用这个域
								// 一、空闲页；二、空闲零化页
		struct swap_page_t *p;	// 如果指向该页面被声明为可交换内存
								// 则该指针指向交换记录。
	};
	uint count;	// 引用计数，记录被映射表项所指向的次数。
};

static struct PFN_t *s_pfn;
static struct PFN_t *s_free_pages;
static struct PFN_t *s_free_zero_pages;
static u32 s_first_page,s_last_page;

static bool_t inited = FALSE;


extern void page_fault();	//在memory.asm中 

#define get_pte(vaddr)	(ptt+((u32)(vaddr)>>12))
#define get_pfn(paddr)	(&s_pfn[(paddr)>>12])

//如果页表不存在就会产生异常。 
#define get_paddr(vaddr)	((*get_pte(vaddr)) & 0xfffff000)

// return ((pte - ptt)<<12); 
#define get_vaddr(pte)	((u32)(pte)<<12)


#define NEED_ZERO_PAGE	1
#define NEED_PAGE		2

static u32 get_free_zero_page()
{
	u32 page_addr;

	if(s_free_zero_pages){
		page_addr = (s_free_zero_pages - s_pfn)<<12;
		s_free_zero_pages = s_free_zero_pages->next;
	}else{
		struct msg_t msg;

		msg.type = NEED_ZERO_PAGE;
		_send_wait_msg(PID_MM,&msg);
	}
	
	return page_addr;
}

static u32 get_free_page()
{
	u32 page_addr;
	
	if(s_first_page != s_last_page){
		page_addr = s_first_page;
		s_first_page += _4K;
	}else if(s_free_pages){
		page_addr = (s_free_pages - s_pfn)<<12;
		s_free_pages = s_free_pages->next;
	}else if(s_free_zero_pages){
		page_addr = (s_free_zero_pages - s_pfn)<<12;
		s_free_zero_pages = s_free_zero_pages->next;
	}else{
		struct msg_t msg;

		msg.type = NEED_PAGE;
		_send_wait_msg(PID_MM,&msg);
	}

	return page_addr;
}

static void put_page(u32 paddr,u32 vaddr)
{
	pte_t *pte = get_pte(vaddr);
	
	*pte = paddr | PAGE_WRITE | PAGE_PRESENT;	//写入物理映射地址，访问属性，存在位 
	
	if(inited){
		printf("++");
		get_pfn(paddr)->count ++;
	}
}

static void decommit_page(pte_t *ppte)
{
	if(*ppte & PAGE_PRESENT){
		get_pfn(*ppte)->count --;
		if(get_pfn(*ppte)->count == 0){
			get_pfn(*ppte)->next = s_free_pages;
			s_free_pages = get_pfn(*ppte);
		}
	}
	*ppte &= 0x00000FFE;
}

void commit_region(struct region_t *region)
{
	pte_t *ppte = region->begin;
	pte_t pte = 0;
	
	if(region->flag & FLAG_PAGE_WRITE)
		pte |= PAGE_WRITE;
	if(region->flag & FLAG_PAGE_USER)
		pte |= PAGE_USER;
	if(region->flag & ATTR_PAGE_ZERO)
		pte |= ATTR_PAGE_ZERO;
	else
		pte |= ATTR_PAGE_GNL;
	for(;ppte != region->end;ppte++)
		*ppte = pte;
}

/*
如果是页目录造成缺页，则说明用户访问了一个错误的空间。
因为如果一个在逻辑上可用的内存页面应该在页表中有所记录。
因此如果页目录中有一项 P = 0，则说明该4M线性内存都不存在。 
*/

#define ERROR_MEMORY		1
#define ERROR_MEMORY_ACCESS	1
#define ERROR_MEMORY_WRITE	2

static void send_error(uint error_code,void *p)
{
	struct msg_t msg;
	msg.type = ERROR_MEMORY;
	msg.p0.uiparam = error_code;
	msg.p1.pparam = p;
	
	_send_wait_msg(PID_MM,&msg);
}

void do_no_page(u32 error_code,u32 cr2)
{
	printf("(solutioning:%x",cr2);
	if(cr2 < KERNEL_SEG){
		uint flag = region_flag(get_cur_proc()->mm.region_hdr,(void*)cr2);
		
		switch(flag & ATTR_PAGE_MASK)
		{
		case ATTR_PAGE_UNKNOW:
			send_error(ERROR_MEMORY_ACCESS,(void*)cr2);
		break;
		case ATTR_PAGE_GNL:
			put_page(get_free_page(),cr2);
		break;
		case ATTR_PAGE_ZERO:
			put_page(get_free_zero_page(),cr2);
		break;
		case ATTR_PAGE_FMAP:
			
		break;
		default:
			assert("should not be here!");
		break;
		}
	}else{
		put_page(get_free_page(),cr2);
	}
	printf(")");
}

void do_wp_page(u32 error_code,u32 cr2)
{
	uint flag = region_flag(get_cur_proc()->mm.region_hdr,(void*)cr2);
	
	printf("(wp:%x",cr2);
	
	if(flag & FLAG_PAGE_WRITE){
		switch(flag & ATTR_PAGE_MASK)
		{
		case ATTR_PAGE_GNL:
		case ATTR_PAGE_ZERO:
			assert("should not use this!");
		break;
		case ATTR_PAGE_FMAP:
			assert("should not use this!");
		break;
		default:
			assert("should not be here!");
		break;
		}
	}else{
		
	}
}

void do_page_fault(u32 error_code)
{
	u32 cr2 = scr2();
	if(error_code)
		do_no_page(error_code,cr2);
	else
		do_wp_page(error_code,cr2);
}

void mm_process()
{
	struct msg_t msg;
	int hmsg;
	int retval;
	while(1)
	{
		hmsg = ipc_recv(&msg);
		switch(msg.type)
		{
		case NEED_PAGE:
			
		break;
		case NEED_ZERO_PAGE:
			
		break;
		default:
			
		break;
		}
		if(hmsg != INVALID_HMSG){
			for_wait_msg(hmsg,retval);
		}
	}
}

// create virtual address space
u32 create_vas()
{
	void *new_pdt = kvirtual_alloc(NULL,_4K);
	memset(new_pdt,0,_2K);
	memcpy(new_pdt+_2K,(void*)(PDT+_2K),_2K);
	((u32*)new_pdt)[512] = get_paddr(new_pdt) | PAGE_WRITE | PAGE_PRESENT;
	return get_paddr(new_pdt);
}

void do_mm_fork(int p_pid,int c_pid)
{
	struct process_t *p_proc = get_proc(p_pid);
	struct process_t *c_proc = get_proc(c_pid);
	
	if(p_proc){
		void *new_pdt;
		
		c_proc->mm.region_hdr = vasm_do_fork(p_proc->mm.region_hdr);
		new_pdt = kvirtual_alloc(NULL,_4K);
		memset(new_pdt,0,_2K);
		memcpy(new_pdt+_2K,(void*)(PDT+_2K),_2K);
		((u32*)new_pdt)[512] = get_paddr(new_pdt);
		c_proc->cr3 = get_paddr(new_pdt);
		
		
	}else{
		c_proc->cr3 = scr3();
		c_proc->mm.region_hdr = create_vasr(NULL,(void*)_4K,(void*)KERNEL_SEG);
	}
}

static region_hdr_t kregion_hdr;

void* kvirtual_alloc(void *addr,size_t size)
{
	return alloc_region(kregion_hdr,addr,size,0);
}

bool_t kvirtual_reset(void *addr)
{
	return reset_region(kregion_hdr,addr,0);
}

bool_t kvirtual_free(void *addr)
{
	return free_region(kregion_hdr,addr);
}

// first：物理内存起始地址
// last ：物理内存结束地址 
void init_mm(u32 first_page,u32 last_page)
{
	int i;
	pte_t *ppte;
	
	inited = FALSE;
	
// 尽快使页故障例程生效
	s_first_page = first_page;
	s_last_page  = last_page;
	
	s_free_pages = NULL;
	s_free_zero_pages = NULL;
	set_sys_idt(INT_VECTOR_PAGE_FAULT,page_fault);

// 初始化kvirtual_*函数系。
	init_vasm();
	kregion_hdr = create_vasr((void*)PTE_BEGIN,(void*)PTE_BEGIN,(void*)PTE_END);
	
// 初始化物理内存数据库
// 得到一段虚拟内存区域，用于存放pfn。
	s_pfn = kvirtual_alloc(NULL,(last_page>>12)*sizeof(struct PFN_t));
	
	memset(s_pfn,0,(last_page>>12)*sizeof(struct PFN_t));

	// 1M 以下的物理内存不使用 
	for(i = _1M>>12;i < s_first_page>>12;i++)
	{
		s_pfn[i].count = 1;
	}

// 使用户区页表不可用 
	printf("ppte:%x\n",ppte);
	ppte = get_pte(ptt);
	memset(ppte,0,sizeof(pte_t)*512);
	
// 使内核区页表可以使用 
	ppte = get_pte(ptt) + 512; 
	for(;ppte != get_pte(ptt)+1024;ppte++)
	{
		if(!(*ppte & PAGE_PRESENT))
			*ppte |= PAGE_WRITE|ATTR_PAGE_GNL;
	}
	
	for(ppte = get_pte(STACK_BOTTOM);ppte != get_pte(STACK_TOP);ppte++)
	{
		if(!(*ppte & PAGE_PRESENT))
			*ppte |= PAGE_WRITE|ATTR_PAGE_GNL;
	}
}
