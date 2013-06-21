
#include "io.h"
#include "string.h"
#include "handle.h"

#define FOT_SIZE		_4K	// FILE OBJECT TABLE SIZE
#define FOT_COUNT		(FOT_SIZE/sizeof(struct file_t))
#define FILE_OBJ_BEGIN	((struct file_t*)0x0)
#define FILE_OBJ_END	(FILE_OBJ_BEGIN + FOT_COUNT) 

struct file_t; 

struct IO_req_t{
  union{
	thread_t *thread;			// 与该请求有关的线程 
	void (*proc)(struct IO_req_t*);
  };
	int status;					// 标记IO请求的状态 
	int ret_val;				// IO设备对于该请求的返回值 
	file_t *fp;					// IO请求对应的文件对象 
	void *buf;					// 请求数据的缓冲区
	union{						// 标示读取位置 
		u32 offset;				// 偏移 
		u32 sect;				// 扇区偏移 
	};
	size_t size;				// 请求数据的大小 
	struct IO_req_t *next;		// 请求链的下一个请求 
};

struct file_t{
	size_t cur_pos;
	size_t size;
	uint flag;					// 关于文件的打开方式 
	uint counter;				// 引用计数 
	struct vcb_t *vcb;
	struct IO_req_t *req;		// 关于这个文件的请求链表
								// 在释放一个文件的时候必须关心这些请求 
	struct file_t *next;		// 用于遍历所有系统已经打开的文件 
};

static struct IO_req_t *s_req_free;	// 用于请求块的内存池 
static struct file_t *s_file_free;	// 用于文件块的内存池
static struct IO_req_t *s_req_head;	// 循环链表 
static struct file_t *s_file_head;	// 循环链表 

// 在已打开的文件中查找 
static handle_t search_file(const char *filename)
{
	struct file_t *file = s_file_head->next;
	while(file != s_file_head)
	{
		file
	}
}

static struct IO_req_t* alloc_req_node()
{
	struct IO_req_t* ret = s_req_free;
	s_req_free = s_req_free->next;
	return ret;
}

static void free_req_node(struct IO_req_t* node)
{
	node->next = s_req_free;
	s_req_free = node;
}

// 暂时我还想不出关闭一个请求是什么意思，只是将它用于句柄类型 
static int close_req(struct IO_req_t* p)
{
	
}

static struct IO_req_t* get_req(h_req_t h_req);
{
	return (struct IO_req_t*)get_knl_obj(h_req,close_req);
}

static struct file_t* alloc_file_node()
{
	struct file_t* file = s_file_free;
	s_file_free = s_file_free->next;
	return file;
}

static void free_file_node(struct file_t* node)
{
	node->next = s_file_free;
	s_file_free = node;
}

static int _close(struct file_t *file)
{
	file->counter--;
	if (file->counter == 0){
		// 关闭文件 
		free_file_node(file);
	}
}

static struct file_t* get_file(handle_t h_file)
{
	return (struct file_t*)get_knl_obj(h_file,_close);
}

int eof(handle_t h_file)
{
	struct file_t *fp = get_file(h_file);
	return (fp->cur_pos == fp->size) 1:0;
}

handle_t _open(const char *filename,u32 flag)
{
	
}

handle_t open(const char *filename,u32 flag)
{
	handle_t handle = search_file(filename);
	if(handle == INVALID_HANDLE){
		return _open(filename,flag);
	}
	return handle;
}

int close(handle_t h_file)
{
	return _close(get_file(h_file));
}

void _sys_read(struct IO_req_t *req)
{
	size_t rb = 0;	// read bytes
	const struct file_t *fp = req->fp;
	if(fp->size - req->offset < req->size)	// 如果文件流逻辑大小不足以读取那么多数据 
		req->size = fp->size - req->offset;

	while(req->offset < fp->vcb->offset)
	{
		fp->vcb = fp->vcb->prev;			// 没有考虑已经释放之前的视图块的情况 
	}

	while(rb != count)
	{
		const size_t min_size = min(count,fp->vcb->size - (fp->cur_pos - fp->vcb->offset));
		memcpy(buf,get_view_buf(fp->vcb)+(fp->cur_pos-fp->vcb->offset),min_size);
		rb += min_size;
	}
	do{
		
	}while();

	req->ret_val += rb;
	if(req->ret_val != req->size){	// 如果没读够则说明缓冲区中的数据不足
									// 需要对应的设备来填充缓冲区，真麻烦呀！ 
		
	}
}

h_req_t sys_read(handle_t h_file,void *buf,size_t size)
{
	struct IO_req_t *req = alloc_req_node();
	const h_req_t h_req = open_handle(req,close_req);
	req->thread = get_cur_thread();
	req->status = REQ_TYPE_REQ;
	req->ret_val = 0;
	req->fp = get_file(h_file);
	req->buf = buf;
	req->offset = req->fp->cur_pos;
	req->size = size;
	req->next = NULL;

	_sys_read(req);
	return h_req;
}

void _sys_write(struct IO_req_t *req) 
{
	
}

h_req_t sys_write(handle_t h_file,void *buf,size_t size)
{
// 构建一个请求 
	struct IO_req_t *req = alloc_req_node();
	const h_req_t h_req = open_handle(req,close_req);
	req->thread = get_cur_thread();
	req->status = REQ_TYPE_REQ;
	req->ret_val = 0;
	req->fp = get_file(h_file);
	req->buf = buf;
	req->offset = req->fp->cur_pos;
	req->size = size;
	req->next = NULL;

	_sys_write(req);
	return h_req;
}

int sys_wait(h_req_t h_req)
{
	struct req_t *req = get_req(h_req);
	
	switch(req->status)
	{
	case REQ_TYPE_REQ:
		req->status = REQ_TYPE_WAIT;
		req->thread = get_cur_thread();
		to_block(req->thread);
		switch_to();	// 该函数不会返回了，
		return 0;		// 为了避免敬告，我在此写了一个返回语句。 
	break;
	case REQ_TYPE_WAIT:
		// error，已经有线程在等待了 
	break;
	case REQ_TYPE_COM:
		const int ret = req->ret_val;
		free_req_node(req);
		close_handle(h_req);
		return ret;
	break;
	}
}

void _sys_com(struct req_t *req,int ret_value)
{
	if(req->next){				// 如果有前序请求 
		req->proc(req->next);
		free_req_node(req);
	}else{
		switch(req->status)
		{
		case REQ_TYPE_REQ:
			req->status = REQ_TYPE_COM;
			req->ret_val = ret_value;
		break;
		case REQ_TYPE_WAIT:
			to_ready(req->thread);
			free_req_node(req);
		break;
		case REQ_TYPE_COM:
			// error
		break;
		}
	}
}

// 其他线程调用这个函数，来启动另一个线程。 
void sys_com(h_req_t h_req,int ret_value)
{
	struct req_t *req = get_req(h_req);
	_sys_com(req,ret_value);
}

long filelength(handle_t h_file)
{
	const file_t *fp = get_file(h_file);
	return (fp != NULL)?fp->size : -1L;
}

int read(handle_t h_file,void *buf,size_t count)
{
	handle_t h_req = sys_read(h_file,buf,count);
	return sys_wait(h_req);
}

int write(handle_t h_file,const void *buf,size_t count)
{
	handle_t h_req = sys_write(h_file,buf,count);
	return sys_wait(h_req);
}

long tell(handle_t h_file)
{
	struct file_t *fp = get_file(h_file);
	
	return (fp == NULL)? -1L:fp->cur_pos;
}

long lseek(handle_t h_file,long offset,int origin)
{
	struct file_t *fp = get_file(h_file);

	if(fp != NULL){
		switch(origin)
		{
		case SEEK_SET:
			fp->cur_pos = offset;
		break;
		case SEEK_CUR:
			fp->cur_pos += offset;
		break;
		case SEEK_END:
			fp->cur_pos = fp->size - offset;
		break;
		}
		return fp->cur_pos;
	}else{	// 无效句柄 
		return -1L;
	}
}

void io_process()
{
	struct msg_t msg;
	int hmsg;
	int retval;

	while(1)
	{
		hmsg = recv_msg(&msg);
		switch(msg.type)
		{
		case SC_CLOSE:
			
		break;
		case SC_EOF:
			retval = eof(msg.p0.iparam);
		break;
		case SC_FILELENGTH:
			
		break;
		case SC_LSEEK:
			retval = lseek(msg.p0.iparam,msg.p1.lparam,msg.p2.iparam);
		break;
		case SC_READ:
			
		break;
		case SC_TELL:
			retval = tell(msg.p0.iparam);
		break;
		case SC_WRITE:
			
		break;
		default:
			
		break;
		}
		if(hmsg != INVALID_HMSG)
			for_wait_msg(hmsg,retval);
	}
}

void init_io()
{
	struct file_t *p = FILE_OBJ_BEGIN;
	s_file_free = p;
	for(;p != (FILE_OBJ_END-1);p++)
	{
		p->next = p+1;
	}
	p->next = NULL;
}
