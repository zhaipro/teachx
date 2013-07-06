
/*
暂时只实现3.5寸1.44M软驱
为了方便在虚拟中实验嘛
如果真的能把这个系统完整，就废除软盘驱动，完全的鸡肋。 
*/

#include "asm.h"
#include "kernel.h"
#include "intc.h"

/* Floppy disk controller regs. */
#define PORT_FD_DOR		0x3f2		/* Digital Output Register, only read 数字控制寄存器 */
#define PORT_FD_STATUS	0x3f4		/* Main Status Register, only read */
#define PORT_FD_DATA	0x3f5		/* read % write */
#define PORT_FD_DIR		0x3f7		/* Digital Input Register, only read */
#define PORT_FD_DCR		0x3f7		/* Diskette Control Register, only write 传输率控制 */

/* 数字输出寄存器定义 DOR(0X03F2) */
#define DRV_SET0	0X00
#define DRV_SET1	0X01
#define DRV_SET2	0X02
#define DRV_SET3	0X03
#define RESET		0X04
#define DMA_INT		0X08
#define MOT_EN0		0X10
#define MOT_EN1		0X20
#define MOT_EN2		0X40
#define MOT_EN3		0X80

/* 主状态寄存器 */
#define STATUS_DAB		0X01
#define STATUS_DBB		0X02
#define STATUS_DCB		0X04
#define STATUS_DDB		0X08 
#define STATUS_BUSY		0X10	//控制器忙 
#define STATUS_NDM		0X20	//0 - DMA mode
#define STATUS_READ		0X40	//1 f->u
#define STATUS_READY	0X80


/* Values for FD_COMMAND */
#define FD_RECALIBRATE	0x07	/* move to track 0 */
#define FD_SEEK			0x0F	/* seek track */
#define FD_READ			0xE6	/* read with MT, MFM, SKip deleted */
#define FD_WRITE		0xC5	/* write with MT, MFM */
#define FD_SENSEI		0x08	/* Sense Interrupt Status */
#define FD_SPECIFY		0x03	/* specify HUT etc */


#define DCR_500		0X00
#define DCR_300		0X01
#define DCR_250		0X02

#define FLOPPY_144_TolSec		2880
#define FLOPPY_144_SecPerTrk	18 
 
static u32 reset;
/*
 * globals used by 'result()'
 */
#define MAX_REPLIES 7
static u8 reply_buffer[MAX_REPLIES];
#define ST0 (reply_buffer[0])
#define ST1 (reply_buffer[1])
#define ST2 (reply_buffer[2])
#define ST3 (reply_buffer[3])

extern void floppy_int();
void (*do_floppy)();

static void kill_motor()
{
	out(0x03f2,0);
}

//floppy disk out
static bool_t fd_out(u8 byte)
{
	int counter;
	u8 status;

	for(counter = 0 ; counter < 10000 ; counter++)
	{
		status = in(PORT_FD_STATUS) & (STATUS_READY | STATUS_READ);
		if (status == STATUS_READY) {
			out(PORT_FD_DATA,byte);
			return TRUE;
		}
	}
	return FALSE;
}


static bool_t fd_recalibrate(u32 drive)
{
	return fd_out(FD_RECALIBRATE) && fd_out(drive);
}

static int fd_result()
{
	int i = 0, counter, status;

	if (reset)
		return -1;
	for (counter = 0 ; counter < 10000 ; counter++) {
		status = in(PORT_FD_STATUS)&(STATUS_READ|STATUS_READY|STATUS_BUSY);
		if (status == STATUS_READY)
			return i;
		if (status == (STATUS_READ|STATUS_READY|STATUS_BUSY)) {
			if (i >= MAX_REPLIES)
				break;
			reply_buffer[i++] = in(PORT_FD_DATA);
		}
	}
	reset = 1;
	printk("Getstatus times out\n\r");
	return -1;
}

void fd_unexpect()
{
	
}

void do_fd_request(u32 block)
{
	block % FLOPPY_144_SecPerTrk;
}

void init_floppy(void)
{
	do_floppy = fd_unexpect;
	set_8259a_idt(INT_FLOPPY,floppy_int);
	enable_irq(INT_FLOPPY);
}
