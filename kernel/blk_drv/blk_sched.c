
#include "config.h"
#include "blk.h"
#include "process.h"
#include "_sys_call.h"

#define MAX_REQUEST	20

struct request_t{
	u32 sector;
	u8 nsect;
	u8 cmd;
	void *buf;
	int mid;
	struct request_t *prev;	// 为了电梯算法 
	struct request_t *next;
};

static struct request_t *s_cur_req = NULL;
static struct request_t *s_free_reqs;


static void hd_do_request()
{
	if(s_cur_req)switch(s_cur_req->cmd)
	{
	case SC_HD_READ:
		hd_read(s_cur_req->buf,s_cur_req->sector,s_cur_req->nsect);
	break;
	case SC_HD_WRITE:
		hd_write(s_cur_req->buf,s_cur_req->sector,s_cur_req->nsect);
	break;
	default:
		
	break;
	}
} 

// 将一个请求项从请求链表中移除 
static void hd_do_next_req()
{
	#define INSIDE	0
	#define OUTSIDE	1
	static int direction = INSIDE;
	struct request_t *req = s_cur_req;
	
	if(req->next == s_cur_req){
		s_cur_req = NULL;
	}else{
		req->next->prev = req->prev;
		req->prev->next = req->next;
		s_cur_req = req->next;
		hd_do_request();
	}
	
	req->next = s_free_reqs;
	s_free_reqs = req;
}

// 将一个新请求项加入请求链表
// 暂时不使用电梯算法，而是FIFO 
static void req_insert(uint sector,uint nsect,uint cmd,void *buf,uint mid)
{
	struct request_t *request = s_free_reqs;

	s_free_reqs = s_free_reqs->next;
	
	request->sector = sector;
	request->nsect = nsect;
	request->cmd = cmd;
	request->buf = buf;
	request->mid = mid;
	
	if(s_cur_req){
		request->prev = s_cur_req->prev;
		request->next = s_cur_req;
		
		s_cur_req->prev->next = request;
		s_cur_req->prev = request;
	}else{
		s_cur_req = request;
		s_cur_req->prev = s_cur_req->next = s_cur_req;
		hd_do_request();
	}
}

#define REQUEST_COM		1001

void end_req(bool_t b)
{
	struct msg_t msg;
	
	msg.type = REQUEST_COM;
	msg.p0.iparam = s_cur_req->mid;
	msg.p1.iparam = b;
	_send_msg(PID_HD,&msg);
	
	hd_do_next_req();
}

void init_blk_sched()
{
	int i;
	
	s_free_reqs = (struct request_t*)kvirtual_alloc(NULL,_4K);
	for(i=0;i<MAX_REQUEST - 1;i++)
		s_free_reqs[i].next =  &s_free_reqs[i];
	s_free_reqs[i].next = NULL;
}

void hd_process()
{
	struct msg_t msg;
	int mid;
	
	init_blk_sched();
	
	while(1)
	{
		mid = ipc_recv(&msg);
		switch(msg.type)
		{
		case SC_HD_READ:
		case SC_HD_WRITE:
		{
			if(s_free_reqs == NULL){
				for_wait_msg(mid,FALSE);
			}else{
				req_insert(msg.p1.uiparam,msg.p2.uiparam,msg.type,msg.p0.pparam,mid);
			}
		}
		break;
		case REQUEST_COM:
			if(msg.p0.iparam != INVALID_HMSG)
				for_wait_msg(msg.p0.iparam,msg.p1.iparam);
		break;
		default:
			
		break;
		}
	}
}

