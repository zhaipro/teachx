
#include "config.h"
#include "mm.h"

#define VIEW_SIZE	(512*_1K)		// 一个视图块的大小 
#define VIEW_PAGES	(VIEW_SIZE/_4K)	// 一个视图块的页面数 

typedef byte view_buf_t[VIEW_SIZE];

struct vcb_t{	//view control block
	u32 offset; //or sector
	size_t size;
	struct vcb_t *prev;
	struct vcb_t *next;
};

#define view_bufs ((view_buf_t*)VIEW_BUF_BEGIN)
struct vcb_t *vcb_head;

// 返回控制块对应的视图 
byte* get_buf(struct vcb_t *vcb)
{
	return view_bufs + (vcb-vcb_head);
}

void init_buffer()
{
	kvirtual_alloc((void*)VIEW_BUF_BEGIN,VIEW_BUF_END - VIEW_BUF_BEGIN);
	vcb_head = kvirtual_alloc(NULL,
				(VIEW_BUF_END-VIEW_BUF_BEGIN)/VIEW_SIZE*sizeof(struct vcb_t));
	
}
