
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
一个控制块对应到的内存块的逻辑结构：
	{[mem_ctl_block][size_of_block][size_of_rest]}

所有的控制块组成双指针循环链表。 
*/

typedef union _mem_ctl_block_t{
	struct{
		union _mem_ctl_block_t *prev;
		union _mem_ctl_block_t *next;
		size_t size_of_block;			// 块使用的大小（单位：字节），对齐到控制块大小 
		size_t size_of_rest;			// 块后空余空间的大小（单位：字节），对齐到控制块大小 
	};
	double t[2];						// 内存对齐 
}mem_ctl_block_t;

static mem_ctl_block_t *blocks = NULL;

// 默认初始化会成功
void init_alloc()
{
	blocks = (mem_ctl_block_t*)malloc(512*sizeof(mem_ctl_block_t));
	blocks->prev = blocks;
	blocks->next = blocks;
	blocks->size_of_block = 0;
	blocks->size_of_rest = (512-1)*sizeof(mem_ctl_block_t);
}

void close_alloc()
{
	free(blocks);
}

void* my_malloc(size_t size)
{
	if(NULL == blocks){			// 如果用户不使用动态分配就尽量不要动这块大内存
		init_alloc();
	}

	mem_ctl_block_t *obj = NULL;	// 后续有足够大空闲空间的块 
	mem_ctl_block_t *temp = blocks;	// 用于循环等 
	size_t max_size = ~0u;
	size = (size+2*sizeof(mem_ctl_block_t)-1) & (~(sizeof(mem_ctl_block_t)-1));

	do{								// 找到一个最适合的块，不大不小 
		if(temp->size_of_rest >= size && temp->size_of_rest < max_size){
			max_size = temp->size_of_rest;
			obj = temp;
		}
		temp = temp->next;
	}while(temp != blocks);

	if(obj){						// 拆分一个控制块，并将新块插入链表 
		temp = obj + obj->size_of_block/sizeof(mem_ctl_block_t) + 1;
		temp->size_of_rest = obj->size_of_rest - size;
		temp->size_of_block = size - sizeof(mem_ctl_block_t);
		obj->size_of_rest = 0;
		temp->prev = obj;
		temp->next = obj->next;
		obj->next->prev = temp;
		obj->next = temp;
		return temp + 1;
	}else{
		return NULL;
	}
}

void my_free(void *block)
{
	mem_ctl_block_t *p = (mem_ctl_block_t*)block - 1;

	p->prev->size_of_rest += sizeof(mem_ctl_block_t) + p->size_of_block + p->size_of_rest;
	p->prev->next = p->next;
	p->next->prev = p->prev;
}

void* my_realloc(void* block,size_t size)
{
	mem_ctl_block_t *p = (mem_ctl_block_t*)block - 1;
	const size_t all_size = p->size_of_block + p->size_of_rest;
	if(size <= all_size){
		p->size_of_block = (size+sizeof(mem_ctl_block_t)-1)&(~(sizeof(mem_ctl_block_t)-1));
		p->size_of_rest = all_size - p->size_of_block;
		return block;
	}else{
		void *ret = my_malloc(size);
		memcpy(ret,block,size);
		my_free(block);
		return ret;
	}
}

int main()
{
	int *p1 = (int*)my_malloc(sizeof(int));
	int *p2 = (int*)my_malloc(sizeof(int));

	scanf("%d",p1);
	printf("%d\n",*p1);
	scanf("%d",p2);
	printf("%d\n",*p2);
//	my_free(p1);
//	my_free(p2);
//	p1 = (int*)my_malloc(sizeof(int));
	printf("%d\n",*p1);
	
	p1 = (int*)my_realloc(p1,10*sizeof(int));
	p1[1] = 2;
	my_free(p1);
	
	printf("%d\n",p1[1]);
	
	close_alloc();
	return 0;
}
