
/*
win代表温切斯特(Winchester)硬盘 
*/
#include "intc.h"
#include "setup.h"
#include "asm.h"
#include "vga.h"
#include "process.h"
#include "_sys_call.h"

#define PORT_HD_DATA	0X1F0	//数据寄存器 
#define PORT_HD_ERROR	0X1F1	//错误寄存器
#define PORT_HD_PRECOMP	0X1F1 
#define PORT_HD_NSECTOR	0X1F2	//扇区数寄存器。完成一个扇区的操作该寄存器就自动减一 
#define PORT_HD_SECTOR	0X1F3	//扇区号寄存器 
#define PORT_HD_LCYL	0X1F4	//柱面号寄存器LOW 
#define PORT_HD_HCYL	0X1F5	//柱面号寄存器HIGH
#define PORT_HD_CURRENT	0X1F6	// 
#define PORT_HD_STATUS	0X1F7
#define PORT_HD_COMMAND	0X1F7 
#define PORT_HD_CMD		0X3F6	

#define WIN_RESTORE	0X10	//默认步进速率 35us 
#define WIN_READ	0X20	//忽略ECC码，允许重试
#define WIN_WRITE	0X30	//忽略ECC码，允许重试
#define WIN_VERIFY	0X40
#define WIN_FORMAT	0X50	//格式化磁道，
#define WIN_INIT	0X60	//控制器初始化
#define WIN_SEEK	0X70
#define WIN_DIAGNOSE 0X90	//驱动器诊断命令 
#define WIN_SPECIFY	0X91 

#define HD_STATUS_ERROR	0X01
#define HD_STATUS_INDEX 0X02
#define HD_STATUS_ECC	0X04
#define HD_STATUS_DRQ	0X08
#define HD_STATUS_SEEK	0X10
#define HD_STATUS_WRERR	0X20
#define HD_STATUS_READY	0X40
#define HD_STATUS_BUSY	0X80
#define HD_STATUS_OK	(HD_STATUS_READY|HD_STATUS_SEEK)

static u8  s_ctl;
static u8  s_pcom;
static u32 s_sec_per_trk;
static u32 s_heads;
 
struct request_t{
	u32 sector;
	u8 nsect;
	u8 cmd;
	void *buf;
	int hmsg;
	struct request_t *prev;	// 为了电梯算法 
	struct request_t *next;
};

static struct request_t *s_cur_req = NULL;
static struct request_t *s_req_head;
static struct request_t *s_free_reqs;
/* 以下代码用于管理请求项链表 ----------------------------------------------- */
// 将一个请求项从请求链表中移除 
static void req_erase(struct request_t *preq)
{
	preq->next->prev = preq->prev;
	preq->prev->next = preq->next;
}

// 将一个新请求项加入请求链表
// 暂时不使用电梯算法，而是FIFO 
static void req_insert(struct request_t *preq) 
{
	preq->prev = s_req_head->prev;
	preq->next = s_req_head;
	
	s_req_head->prev->next = preq;
	s_req_head->prev = preq;
}

static struct request_t* req_next()
{
#define INSIDE	0
#define OUTSIDE	1
	static int direction = INSIDE;
}

/* -------------------------------------------------------------------------- */ 

extern void hd_int();

void hd_bad_int();				//错误的中断调用 
void (*do_hd)() = hd_bad_int;
void (*do_floppy) = hd_bad_int;	//暂时放在这里一下。 

static u32 hd_ctrl_ready()
{
	u32 retries=10000;

	while (--retries && (in_p(PORT_HD_STATUS)&0xc0)!=0x40)
		;	//在这里可以写入nop 
	return (retries);
}

//FALSE 表示 控制器就绪
//TRUE  表示 控制器忙
static bool_t hd_ctrl_busy()
{
	u32 i;
	for(i=0;i<10000;i++)
		if(HD_STATUS_OK == (in_p(PORT_HD_STATUS)&(HD_STATUS_BUSY|HD_STATUS_OK)))
			return FALSE;
	return TRUE;
}

//返回值：
//0表示正常，1表示出错 
static u32 win_result()
{
	const u8 status = in(PORT_HD_STATUS);
	return (status & (HD_STATUS_BUSY|HD_STATUS_READY|HD_STATUS_WRERR|	
		HD_STATUS_SEEK|HD_STATUS_ERROR))!=(HD_STATUS_SEEK|HD_STATUS_READY);
}

//hard disk out
static void hd_out(u32 sector,u8 nsect,u8 cmd)
{
	u32 trk = sector / s_sec_per_trk;
	u32 cyl = trk / s_heads;
	
	if(hd_ctrl_ready()){
		console_write("error:console_ready");
	}else{
		console_write("console_ready");
	}
	
	out_p(PORT_HD_CMD,s_ctl);
	out_p(PORT_HD_PRECOMP,s_pcom);
	out_p(PORT_HD_NSECTOR,nsect);
	out_p(PORT_HD_SECTOR,(sector % s_sec_per_trk) + 1);
	out_p(PORT_HD_LCYL,cyl&0xff);
	out_p(PORT_HD_HCYL,(cyl>>8)&0xff);
	out_p(PORT_HD_CURRENT,0xa0 | (trk % s_heads));	//第0个驱动器 
	out_p(PORT_HD_COMMAND,cmd);
}

void hd_bad_int()
{
	
}

#define REQUEST_COM		1001

static void hd_read(u32 sector,u8 nsect);
static void hd_write(u32 sector,u8 nsect);

void hd_do_request()
{
	if(s_cur_req)switch(s_cur_req->cmd)
	{
	case SC_HD_READ:
		hd_read(s_cur_req->sector,s_cur_req->nsect);
	break;
	case SC_HD_WRITE:
		hd_write(s_cur_req->sector,s_cur_req->nsect);
	break;
	default:
		
	break;
	}
} 

static void end_req(bool_t b)
{
	struct msg_t msg;
	struct request_t *p_next_req;

	msg.type = REQUEST_COM;
	msg.p0.iparam = s_cur_req->hmsg;
	msg.p1.iparam = b;
	send_msg(PID_HD,&msg,FALSE);
	
	p_next_req = req_next();
	
	req_erase(s_cur_req);
	s_cur_req->next = s_free_reqs;
	s_free_reqs = s_cur_req;
	s_cur_req = p_next_req;
}

static void hd_read_int()
{
	console_write("s31");
	if(win_result()){
		console_write("read error\n");
		return;
	}

	port_readw(PORT_HD_DATA,s_cur_req->buf,256);

	if(in(PORT_HD_NSECTOR)){	//剩余扇区数 
		s_cur_req->buf += 512;
	}else{
		hd_do_request();
	}

	eoi_m();
	eoi_s();
} 

static void hd_read(u32 sector,u8 nsect)
{
	do_hd = hd_read_int;
	hd_out(sector,nsect,WIN_READ);
}

void hd_write_int()
{
	console_write("write 1");
	if(win_result()){
		console_write("write error");
		return;
	}
	if(in(PORT_HD_NSECTOR)){	//剩余扇区数 
		s_cur_req->buf += 512;
		port_writew(PORT_HD_DATA,s_cur_req->buf,256);
	}else{
		console_write("write ok");
		hd_do_request();
	}

	eoi_m();
	eoi_s();
}

//向硬盘发送读取请求 
void hd_write(u32 sector,u8 nsect)
{
	u32 i;
	u8 r;
	do_hd = hd_write_int;
	hd_out(sector,nsect,WIN_WRITE);
	for(i=0;i<3000 && !(r=in_p(PORT_HD_STATUS)&HD_STATUS_DRQ);i++)
		;
	if(!r)
		end_req(FALSE);
	else
		port_writew(PORT_HD_DATA,s_cur_req->buf,256);
}

void init_hd()
{
	s_ctl = g_sys_info->hd0[8];
	s_pcom = (g_sys_info->hd0[6]<<6) | (g_sys_info->hd0[5]>>2);
	s_sec_per_trk = g_sys_info->hd0[0x0e];
	s_heads = g_sys_info->hd0[2];
	set_8259a_idt(INT_AT,hd_int);
	enable_irq(2);		//打开级联中断 
	enable_irq(INT_AT);
}

void hd_process()
{
	struct msg_t msg;
	int hmsg;
	
	init_hd();
	
	while(1)
	{
		hmsg = ipc_recv(&msg);
		switch(msg.type)
		{
		case SC_HD_READ:
		case SC_HD_WRITE:
		{
			if(s_free_reqs == NULL){
				for_wait_msg(hmsg,FALSE);
			}else{
				struct request_t *request = s_free_reqs;
				
				s_free_reqs = s_free_reqs->next;
				
				request->sector = msg.p1.uiparam;
				request->nsect = msg.p2.uiparam;
				request->cmd = msg.type;
				request->buf = msg.p0.pparam;
				request->hmsg = hmsg;
				
				req_insert(request);				
				
				if(s_cur_req == NULL){
					s_cur_req = request;
					hd_do_request();
				}
			}
		}
		case REQUEST_COM:
			if(msg.p0.iparam != INVALID_HMSG)
				for_wait_msg(msg.p0.iparam,msg.p1.iparam);
		break;
		default:
			
		break;
		}
	}
}
