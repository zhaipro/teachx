
/*
在这里所有关于内存的数据都是以页为单位。 
vaddr 代表虚拟地址或线性地址
paddr 代表物理地址（页框地址） 
*/

#include "type.h"
#include "kernel.h"
#include "memory.h"
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

/* 软件解释 之 此页已存在 *
 * 只可以使用那三个处理器 *
 * 为用户保留的标志位     */
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


extern void page_fault();	//在memory.asm中 

#define get_pte(vaddr)	(ptt+((u32)(vaddr)>>12))		// 求下界 
#define get_ptec(vaddr)	get_pte((u32)(vaddr)+_4K-1)		// 求上界 
#define get_pfn(paddr)	(&s_pfn[(paddr>>12)])

//如果页表不存在就会产生异常。 
#define get_paddr(vaddr)	((*get_pte(vaddr)) & 0xfffff000)

// return ((pte - ptt)<<12); 
#define get_vaddr(pte)	((u32)(pte)<<12)


#define page_zero(addr) memset((void*)addr,0,_4K)

#define NEED_ZERO_PAGE	1
#define NEED_PAGE		2

static u32 get_free_zero_page()
{
	u32 page_addr;
	if(s_free_zero_pages){
		page_addr = (s_free_zero_pages - s_pfn)<<12;
		s_free_zero_pages = s_free_zero_pages->next;
		return page_addr;
	}else{
		struct msg_t msg;

		msg.type = NEED_ZERO_PAGE;
		_send_wait_msg(PID_MM,&msg);
	}
}

static u32 get_free_page()
{
	u32 page_addr;
	if(s_free_pages){
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
	
	*pte = paddr | (*pte&0xfff) | PAGE_PRESENT;	//写入物理映射地址，访问属性，存在位 
}

static void decommit_page(pte_t *ppte)
{
	if(*ppte & PAGE_PRESENT){
		s_pfn[*ppte>>12].count --;
		if(s_pfn[*ppte>>12].count == 0){
			s_pfn[*ppte>>12].next = s_free_pages;
			s_free_pages = &s_pfn[*ppte>>12];
		}
	}
	*ppte &= 0x00000fff;
}

// 使用与malloc同样的思想管理虚拟内存 
// 已使用块[begin,end)
// 空闲块  [end,end2) 
struct region_t{
// 这里没有定义 ATTR_RT_READ ，这是因为这会让用户以为，如果他们不设置该标志，那该区段就不可读。
// 然而这是错误的认为。 
#define ATTR_RT_WRITE	0X01
#define ATTR_RT_USER	0X02

#define ATTR_RT_ZERO	0X04
#define ATTR_RT_FMAP	0X08
#define ATTR_RT_COW		0X40

	u32 attr;
	pte_t *begin;
	pte_t *end;
	pte_t *end2; 

	struct region_t *prev;
	struct region_t *next;
};

/*
进程需要虚拟内存管理，来提供一个平台，但是虚拟内存管理也需要内存空间呀！！！
当还没有虚拟内存管理的时候，虚拟内存管理程序的内存怎么办呢？？？
答案是手工分配。
具体方法：
	初始化一个内存池。 
*/
#define SIZE_REGION_BUFFER	_4K
#define COUNT_REGION_NODE	(SIZE_REGION_BUFFER/sizeof(struct region_t))
static struct region_t s_regions[COUNT_REGION_NODE];
#define BEGIN_REGION_BUFFER	s_regions
#define END_REGION_BUFFER	(BEGIN_REGION_BUFFER + COUNT_REGION_NODE)

static struct region_t *s_free_regions = NULL;

// 返回addr所在的区域(已保留区域)
static struct region_t* get_region(uint pid,void* addr)
{
	struct region_t *proc_regions = (struct region_t *)get_proc_regions(pid);
	struct region_t *p = proc_regions;
	pte_t *ppte = get_pte(addr);

	do{
		if(ppte < p->end){
			return (ppte >= p->begin)? p:NULL;
		}
		p = p->next;
	}while(p != proc_regions);

	return NULL;
}

static void free_region(struct region_t* region)
{
	region->prev->end2 = region->end2;
	region->prev->next = region->next;
	region->next->prev = region->prev;
	
	region->next = s_free_regions;
	s_free_regions = region; 
}

bool_t release_region(uint pid,void *addr)
{
	struct region_t *region = get_region(pid,addr);
	if(region == NULL || region->begin!=get_pte(addr))
		return FALSE;
	if(!(region->attr&ATTR_RT_USER))
		return FALSE;

	if(region->attr&ATTR_RT_FMAP){
		if(region->attr&ATTR_RT_COW){
			while(region->begin!=region->end)
			{
				if(*region->begin & ATTR_PAGE_COPYED)
					decommit_page(region->begin);
				*region->begin = 0;
				region->begin++;
			}
		}else{
			while(region->begin!=region->end)
			{
				*region->begin = 0;
				region->begin++;
			}
		}
	}else while(region->begin!=region->end)	// 普通页面 
	{
		decommit_page(region->begin);
		*region->begin = 0;
		region->begin++;
	}

	free_region(region);
	return TRUE;
}

void* reserve_region(uint pid,void *addr,size_t size,u32 flag)
{
	pte_t *p_begin = get_pte(addr);
	pte_t *p_end   = get_ptec(addr + size);
	struct region_t *proc_regions = (struct region_t *)get_proc_regions(pid);
	struct region_t *temp = proc_regions;
	struct region_t *obj = NULL;
	if(addr){
		do{
			if(p_begin >= temp->end){
				if(p_end < temp->end2){
					obj = temp;
					break;
				}
			}
			temp = temp->next;
		}while(temp != proc_regions);
	}else{
		size_t max_size = ~0u;
		do{
			if((temp->end2-temp->end) > (p_end-get_pte(0)) && (temp->end2-temp->end) < max_size){
				max_size = (temp->end2-temp->end);
				obj = temp;
			}
			temp = temp->next;
		}while(temp != proc_regions);
		
		if(obj == NULL)
			return NULL;
		
		p_begin = obj->end;
		p_end = p_begin + (p_end-get_pte(0));
		temp = obj;
	}
	
	if(obj){		
		temp = s_free_regions;
		s_free_regions = s_free_regions->next;

		temp->attr = flag;
		temp->begin = p_begin;
		temp->end = p_end;
		temp->end2 = obj->end2;
		temp->prev = obj;
		temp->next = obj->next;
		obj->next->prev = temp;
		obj->end2 = p_begin;
		obj->next = temp;
		return get_vaddr(temp->begin);
	}else{
		return NULL;
	}
}

void commit_region(struct region_t *region)
{
	pte_t *ppte = region->begin;
	pte_t pte = 0;
	
	if(region->attr & ATTR_RT_WRITE)
		pte |= PAGE_WRITE;
	if(region->attr & ATTR_RT_USER)
		pte |= PAGE_USER;
	if(region->attr & ATTR_RT_ZERO)
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
	int hmsg;
	msg.type = ERROR_MEMORY;
	msg.p0.uiparam = error_code;
	msg.p1.pparam = p;
	
	hmsg = send_msg(PID_MM,&msg,TRUE);
	wait_msg(hmsg,FALSE);
}

void do_no_page(u32 error_code,void *cr2)
{
//增加页错误次数记录 
	pte_t *ppte = get_pte(cr2);

	switch(*ppte & 0xfffff000)
	{
	case ATTR_PAGE_UNKNOW:
	{
		struct region_t *region = get_region(get_cur_pid(),cr2);
		if(region){
			commit_region(region);
			do_no_page(error_code,cr2);
		}else{
			send_error(ERROR_MEMORY_ACCESS,cr2);
		}
	}
	break;
	case ATTR_PAGE_GNL:
		commit_page(cr2);
	break;
	case ATTR_PAGE_ZERO:
		commit_zero_page(cr2);
	break;
	case ATTR_PAGE_FMAP:
		
	break;
	default:
		assert("should not be here!");
	break;
	}
}

void do_wp_page(u32 error_code,void* cr2)
{
	pte_t *ppte = get_pte(cr2);
	switch(*ppte & 0X600)
	{
	case 0:
		send_error(ERROR_MEMORY_WRITE,cr2);
	break;
	case ATTR_PAGE_FWRITE:
		assert("should not use this!");
	break;
	case ATTR_PAGE_FCOW:
		assert("should not use this!");
	break;
	case ATTR_PAGE_PCOW:
		assert("should not use this!");
	break;
	default:
		assert("should not be here!");
	break;
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
	void *new_pdt;
	memset(new_pdt,0,_2K);
	memcpy(new_pdt+_2K,PDT+_2K,_2K);
	return get_paddr(new_pdt);
}

// 初始化必要的用户区段 
void init_user_space(uint pid)
{
	struct region_t *proc_regions = s_free_regions;
	s_free_regions = s_free_regions->next;
	
	proc_regions->attr = 0;
	proc_regions->begin = get_pte(NULL);
	proc_regions->end = get_pte(_4K);
	proc_regions->end2 = get_pte(KERNEL_SEG);
	proc_regions->next = proc_regions->prev = proc_regions;
	
	set_proc_regions(pid,proc_regions);
}

// first：物理内存起始地址
// last ：物理内存结束地址 
void init_mm(u32 first_page,u32 last_page)
{
	int i;
	pte_t *ppte;
	struct region_t *pregion;
	
// 首先从PTE区分配一段虚拟内存，并使其映射物理内存。
	pdt[(KERNEL_SEG+16*_1M)>>22] = first_page | PAGE_PRESENT;
	first_page += _4K;
	ppte = get_pte(KERNEL_SEG+16*_1M);
	page_zero(ppte);
	for(i = 0;i < ((last_page>>12)*sizeof(struct PFN_t)+_4K-1)/_4K;i++)
	{
		ppte[i] = first_page | PAGE_PRESENT;
		first_page += _4K;
	}
	
// 初始化物理内存帧数据库
	s_pfn = (struct PFN_t*)(KERNEL_SEG+16*_1M);
	memset(s_pfn,0,(last_page>>12)*sizeof(struct PFN_t));
	
	s_free_zero_pages = NULL;

// 1M 以下的物理内存不使用 
	for(i = _1M>>12;i < first_page>>12;i++)
	{
		s_pfn[i].count = 1;
	}
	s_free_pages = &s_pfn[i];
	for(;i < (last_page>>12)-1;i++)
	{
		s_pfn[i].next = &s_pfn[i+1];
	}
	s_pfn[i].next = NULL;

// 使页故障中断可以使用 
	set_sys_idt(INT_VECTOR_PAGE_FAULT,page_fault);

// 使用户区页表不可用 
	ppte = get_pte(ptt);
	memset(ppte,0,sizeof(pte_t)*512);
// 使内核区页表可以使用 
	ppte = get_pte(ptt) + 512; 
	for(;ppte != get_pte(ptt)+1024;ppte++)
	{
		if(!(*ppte & PAGE_PRESENT))
			*ppte |= PAGE_WRITE|ATTR_PAGE_GNL;
	}
	
	for(ppte = get_pte(STACK_BOTTOM);ppte != get_pte(KERBEL_END);ppte++)
	{
		if(!(*ppte & PAGE_PRESENT))
			*ppte |= PAGE_WRITE|ATTR_PAGE_GNL;
	}

	s_first_page = first_page;
	s_last_page  = last_page;

	s_free_regions = NULL;
	for(pregion=BEGIN_REGION_BUFFER;pregion!=END_REGION_BUFFER;pregion++)
	{
		pregion->next = s_free_regions;
		s_free_regions = pregion;
	}
}
