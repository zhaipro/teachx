
#include "config.h"
#include "vasm.h"
#include "mm.h" 

#define INIT_BLOCK_SIZE	10

static struct region_t init_block[INIT_BLOCK_SIZE];
static uint s_free_count = 0;
static struct region_t *s_free_regions = NULL;

void init_vasm()
{
	int i;
	
	for(i=0;i<INIT_BLOCK_SIZE-1;i++)
	{
		init_block[i].next = &init_block[i+1];
	}
	init_block[i].next = NULL;
	
	s_free_regions = init_block;
	s_free_count = INIT_BLOCK_SIZE;
}

static struct region_t* get_node()
{
	static bool_t fixing = FALSE;
	struct region_t *new_node;
	
	if(s_free_count < INIT_BLOCK_SIZE && fixing == FALSE){
		int i;
		struct region_t *region;
		
		fixing = TRUE;	
		region = (struct region_t*)kvirtual_alloc(NULL,_4K);
	
		for(i=0;i < _4K/sizeof(struct region_t) - 1;i++)
			region[i].next = &region[i+1];
		
		s_free_count += _4K/sizeof(struct region_t);
		region[i].next = s_free_regions;
		s_free_regions = region;
		
		fixing = FALSE;
	}
	
	new_node = s_free_regions;
	s_free_regions = new_node->next;
	s_free_count --;
	
	return new_node;
}

// 返回addr所在的区域(已保留区域)
static struct region_t* get_region(region_hdr_t hdr,const void* addr)
{
	struct region_t *p = hdr;

	do{
		if(addr < p->end){
			return (addr >= p->begin)? p:NULL;
		}
		p = p->next;
	}while(p != hdr);

	return NULL;
}

region_hdr_t create_vasr(void *begin,void *end,void *end2)
{
	struct region_t *region = get_node();

	region->flag = 0;
	region->prev = region->next = region;
	region->begin = begin;
	region->end = end;
	region->end2 = end2;
	
	return region;
}

region_hdr_t vasm_do_fork(region_hdr_t hdr)
{
	struct region_t *new_hdr;
	struct region_t *p,*q;
	
	new_hdr = get_node();
	new_hdr->prev = new_hdr->next = new_hdr;
	new_hdr->flag = hdr->flag;
	new_hdr->begin = hdr->begin;
	new_hdr->end = hdr->end;
	new_hdr->end2 = hdr->end2;
	
	p = hdr->next;
	while(p != hdr)
	{
		q = get_node();
		
		q->flag = p->flag;
		q->begin = p->begin;
		q->end = p->end;
		q->end2 = p->end2;
		q->prev = new_hdr->prev;
		q->next = new_hdr;
		new_hdr->prev->next = q;
		new_hdr->prev = q;
		
		p = p->next;
	}
}

bool_t free_region(region_hdr_t hdr,void *addr)
{
	struct region_t *region = get_region(hdr,addr);
	
	region->prev->end2 = region->end2;
	region->prev->next = region->next;
	region->next->prev = region->prev;
	
	region->next = s_free_regions;
	s_free_regions = region;
	
	s_free_count ++;
}

void* alloc_region(region_hdr_t hdr,void *addr,size_t size,page_fault_t page_fault)
{
	void *begin;
	void *end;
	struct region_t *insert = NULL;
	
	// 函数get_node可能会间接调用alloc_region函数，因此必须在寻找插入点之前调用 
	struct region_t *new_region = get_node();
	
	if(addr){
		struct region_t *p = hdr;
		begin = (void*)((u32)addr & 0xfffff000);
		end = (void*)(((u32)addr+size+_4K-1) & 0xfffff000);
		
		do{
			if(begin < p->end2){
				if(end <= p->end2)
					insert = p;
				break;
			}
			p = p->next;
		}while(p != hdr);
	}else{
		size_t max_size = ~0u;
		struct region_t *p = hdr;
		size = (size+_4K-1)&0xfffff000;
		
		do{
			if((p->end2-p->end) > size && (p->end2-p->end) < max_size){
				max_size = p->end2-p->end;
				insert = p;
				begin = p->end;
				end = begin + size;
			}
			p = p->next;
		}while(p != hdr);
	}
	
	if(insert){
		new_region->flag = 0;
		new_region->begin = begin;
		new_region->end = end;
		new_region->end2 = insert->end2;
		new_region->prev = insert;
		new_region->next = insert->next;
		new_region->page_fault = page_fault;
		insert->next->prev = new_region;
		insert->next = new_region;
		insert->end2 = begin;
		
		return new_region->begin;
	}else{
		return NULL;
	}
}

bool_t reset_region(region_hdr_t hdr,const void *addr,uint flag)
{
	
}

uint region_flag(region_hdr_t hdr,const void *addr)
{
	struct region_t *region = get_region(hdr,addr);
	
	return region? region->flag : 0;
}
