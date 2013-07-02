
#include "mm.h"
#include "kernel.h"
#include "handle.h" 

// 句柄项类型，好奇怪的名字 
struct handle_elem_t{
	void *knl_obj;	// 指向内核对象
					// 标示内核对象的类型 
	fp_close_handle_t close_proc;
};

#define PHT_SIZE	_4K
#define PHT_COUNT	(PHT_SIZE/sizeof(struct handle_elem_t))
#define PHT_BEGIN	((struct handle_elem_t*)HANDLE_TABLE)	// HANDLE_TABLE 在 kernel.h 中定义 
#define PHT_END		(PHT_BEGIN + PHT_COUNT)

void init_handle_table()
{
	virtual_alloc(PHT_BEGIN,PHT_SIZE,FLAG_PAGE_WRITE);
}

// 使用自由链表的话需要一次初始化，暂时就这样吧！
// 自由链表一点也不难 
static struct handle_elem_t* alloc_handle_elem()
{
	struct handle_elem_t* p = PHT_BEGIN;

	while(p != PHT_END)
	{
		if(p->close_proc == NULL){
			return p;
		}
	}
	return NULL;
}

static void free_handle_elem(struct handle_elem_t *p)
{
	p->close_proc = NULL;
}

// safe
static struct handle_elem_t* get_handle_elem(handle_t handle)
{
	return (handle>=0 && handle < PHT_COUNT)?PHT_BEGIN + handle:NULL;
}

bool_t close_handle(handle_t handle)
{
	struct handle_elem_t *p = get_handle_elem(handle);
	if(p != NULL){
		p->close_proc(p->knl_obj);
		free_handle_elem(p);
		return true;
	}else{
		return false;
	}
}

// 只可以在进程结束的时候调用这个函数。 
void all_close_handle()
{
	handle_elem_t *h;
	for(h=PHT_BEGIN;h!=PHT_END;h++)
	{
		if(h->close_proc != NULL)
			h->close_proc(p->knl_obj);
	}
}

// 得到调用该函数的进程与handle对应的内核对象指针
// close_proc 用于检验 
void* get_knl_obj(int pid,handle_t handle,fp_close_handle_t close_proc)
{
	const struct handle_elem_t *p = get_handle_elem(pid,handle);
	return (p!=NULL && p->close_proc==close_proc)? p->knl_obj:NULL;
}

handle_t open_handle(void *obj,fp_close_handle_t close_proc)
{
	handle_elem_t *p = alloc_handle_elem();
	if(p != NULL){
		p->knl_obj = obj;
		p->close_proc = close_proc;
		return p - PHT_BEGIN;
	}else{	// 没有足够的空间存储句柄了 
		return INVALID_HANDLE;
	}
}
