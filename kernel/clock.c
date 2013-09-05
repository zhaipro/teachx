
#include "_process.h"
#include "_sys_call.h"
#include "asm.h"
#include "config.h"
#include "intc.h"
#include "teachx.h"
#include "type.h"
#include "time.h"
#include "assert.h"


#define PORT_CLOCK_0 0X40
#define PORT_CLOCK_1 0X41			// 实际上我们不会设置计数器1和2 
#define PORT_CLOCK_2 0X42			// 只是为了完整性才列在这里 
#define PORT_CLOCK_MODE	0X43

#define CLOCK_HZ	11931800
#define LATCH		(CLOCK_HZ/100)

/*
端口0x70用来设置CMOS数据地址
端口0x71用来读写0x70设置的地址中的数据单元字节内容 
*/
static u8 cmos_read(u8 addr)
{
	out(0x70,addr);
	return in(0x71);
}

static u8 bcd_to_bin(u8 val)
{
	return val&0xf + (val>>4)*10;
}

static clock_t s_clock = 0;
static clock_t s_wait = 0;


#define SC_CLK_FOR_SLEEP	10001

void do_clock_int()
{
	s_clock ++;
	
	if(s_wait != 0 && s_clock > s_wait){
		struct msg_t msg;
		msg.type = SC_CLK_FOR_SLEEP;
		_send_msg(PID_CLK,&msg,FALSE);
		s_wait = 0;
	}
	
	eoi_m();
}

void init_clock()
{	
// 初始化定时器
	out_p(PORT_CLOCK_MODE,0x36);
	out_p(PORT_CLOCK_0,LATCH & 0xff);
	out_p(PORT_CLOCK_0,LATCH>>8);
	
extern void clock_int();
	set_8259a_idt(INT_CLOCK,clock_int);
	enable_irq(INT_CLOCK);
}

static time_t sys_time()
{
	struct tm t;

// 设置开机时间 
	do{
		// 这里获取的值均是BCD码 
		t.tm_sec = cmos_read(0);			// 0 - 59 
		t.tm_min = cmos_read(2);			// 0 - 59 
		t.tm_hour = cmos_read(4);			// 0 - 23 
		t.tm_mday = cmos_read(7);			// 1 - 31
		t.tm_mon = cmos_read(8);			// 1 - 12
		t.tm_year = cmos_read(9);			//  
	}while(t.tm_sec != cmos_read(0));
	
	t.tm_sec = bcd_to_bin(t.tm_sec);
	t.tm_min = bcd_to_bin(t.tm_min);
	t.tm_hour = bcd_to_bin(t.tm_hour);
	t.tm_mday = bcd_to_bin(t.tm_mday);
	t.tm_mon = bcd_to_bin(t.tm_mon);
	t.tm_year = bcd_to_bin(t.tm_year);
	
	t.tm_mon--;								// 0 - 11
	return mktime(&t);
}

struct for_sleep_t{
	int mid;
	clock_t sleep_to_clock;
	struct for_sleep_t *prev;
	struct for_sleep_t *next;
};

static struct for_sleep_t *head;
static struct for_sleep_t *free;

void init_list(struct for_sleep_t *list,int count)
{
	int i;
	
	head = list;
	head->next = head->prev = head;
	head->mid = INVALID_HMSG;
	head->sleep_to_clock = 0;
	
	free = &list[1];
	for(i=2;i < count;i++)
		list[i - 1].next = &list[i];
	list[count-1].next = NULL;
}

static struct for_sleep_t* top()
{
	return head->next;
}

static void insert(int mid,clock_t sleep_to_clock)
{
	struct for_sleep_t *item;
	struct for_sleep_t *new_item;
	
	assert(head);
	item = head->prev;
	assert(item);
	while(item->sleep_to_clock > sleep_to_clock)
	{
		item = item->prev;
		assert(item);
	}
	
	assert(free);
	new_item = free;
	free = free->next;
	new_item->mid = mid;
	new_item->sleep_to_clock = sleep_to_clock;
	new_item->next = item->next;
	new_item->prev = item;
	item->next->prev = new_item;
	item->next = new_item;
	
	s_wait = top()->sleep_to_clock;
}

static void pop()
{
	struct for_sleep_t *t = top();
	
	assert(t);
	t->next->prev = t->prev;
	t->prev->next = t->next;
	
	t->next = free;
	free = t;
	
	s_wait = top()->sleep_to_clock;
}

void clock_process()
{
	time_t startup_time;	// 从1970年1月1日0时到开机当日经过的秒数 
	clock_t startup_clock[MAX_PROC_COUNT];
	struct for_sleep_t list[MAX_THREAD_COUNT + 1];
	
	
	struct msg_t msg;
	int mid;
	int retval;
	
	startup_time = sys_time();
	
	init_list(list,MAX_THREAD_COUNT + 1);
	
	while(1)
	{
		mid = ipc_recv(&msg);
		switch(msg.type)
		{
		case SC_CLK_CLOCK:
			retval = s_clock - startup_clock[msg.pid_from];
		break;
		case SC_CLK_INIT:
			startup_clock[msg.pid_from] = s_clock;
		break;
		case SC_CLK_SLEEP:
			insert(mid,s_clock + msg.p0.uiparam);
			mid = INVALID_HMSG;
		break;
		case SC_CLK_TIME:
			retval = sys_time();
		break;
		case SC_CLK_FOR_SLEEP:
			while(top()->sleep_to_clock != 0 && top()->sleep_to_clock < s_clock)
			{
				ipc_for_wait(top()->mid,0);
				pop();
			}
		break;
		default:
		break;
		}
		if(mid != INVALID_HMSG)
			ipc_for_wait(mid,retval);
	}
}
