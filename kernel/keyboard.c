
/*
很好奇，无论PS/2还是USB键盘都可以使用0x60 0x64 端口
按键->8048->8042->8259A->系统->(0X60/0X64) 
*/
#include "intc.h"
#include "keyboard.h"
#include "vga.h"	//暂时引用 
#include "kernel.h"

#define OUT_BUFFER	0X60
#define IN_BUFFER	0X60
#define STATUS_REG	0X64
#define CTRL_REG	0X64

void int_key();

#define isbreak(scan) ((scan)&0x80)
#define ismake(scan)  (!isbreak(scan))

static keyboard_buf s_kb_buf;
//在这里我区别对待左右两个同功能的按键，不是真的想区别对待，而是为了确定用户按键状态。 
static bool_t s_shift_l = FALSE,s_shift_r = FALSE;
static bool_t s_alt_l = FALSE,s_alt_r = FALSE;
static bool_t s_ctrl_l = FALSE,s_ctrl_r = FALSE;

#define SHIFT_L 0X2A
#define SHIFT_R 0X36
#define ALT_E   0X38
#define CTRL_E  0X1D
#define BREAK   0X80

//ASCII 1b == esc
static char s_keymap1[128] = {		//普通版本 
 0,  27, '1', '2', '3', '4', '5', '6',  '7', '8', '9', '0', '-', '=','\b','\t',	//0
 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[', ']','\n', 0, 'a', 's',
 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,'\\', 'z', 'x', 'c', 'v',
 'b', 'n', 'm', ',', '.', '/', 0, '*',  0, ' ', 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, '-', 0, 0, 0, '+', 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0		//7
};

static u32 s_keymap2[128] = {		//shift版本 
 0, 27, '!', '@', '#', '$', '%', '^',  '&', '*', '(', ')', '_', '+','\b','\t',	//0
 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  'O', 'P', '{', '}', 0, 0, 'A', 'S',
 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V',
 'B', 'N', 'M', '<', '>', '?', 0, '*',  0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, '-', 0, 0, 0, '+', 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0	//7
};

void init_keyboard()
{
	set_8259a_idt(INT_KEY,int_key);
	s_kb_buf.p_head = s_kb_buf.p_tail = s_kb_buf.buf;
	enable_irq(INT_KEY);
}

static void buf_write(u8 scan_code)
{
	static char msg[2] = "m";	//暂时 
	if(s_kb_buf.p_tail+1 == s_kb_buf.buf+KB_IN_BYTES){
		if(s_kb_buf.p_head == s_kb_buf.buf){
			return;
		}
	}else if(s_kb_buf.p_tail+1 == s_kb_buf.p_head){
		return;
	}
	
	if(s_shift_r || s_shift_l){
		*s_kb_buf.p_tail = s_keymap2[scan_code];
		msg[0] = s_keymap2[scan_code];
	}else{
		*s_kb_buf.p_tail = s_keymap1[scan_code];
		msg[0] = s_keymap1[scan_code];
	}

	++s_kb_buf.p_tail;
	if(s_kb_buf.p_tail == s_kb_buf.buf+KB_IN_BYTES){
		s_kb_buf.p_tail = s_kb_buf.buf;
	}	

	console_write(msg);
}

void do_int_key()
{
	u8 scan_code = in(IN_BUFFER);
	
	switch(scan_code)
	{
	case CTRL_E:
		s_ctrl_l = TRUE;
	break;
	case SHIFT_L:
		s_shift_l = TRUE;
	break;
	case SHIFT_R:
		s_shift_r = TRUE;
	break;
	case ALT_E:
		s_alt_l = TRUE;
	break;
	case 0xe0:
		scan_code = in(IN_BUFFER);
		switch(scan_code)
		{
		case CTRL_E:
			s_ctrl_r = TRUE;
		break;
		case ALT_E:
			s_alt_r = TRUE;
		break;
		case CTRL_E|BREAK:
			s_ctrl_r = FALSE;
		break;
		case ALT_E|BREAK:
			s_alt_r = FALSE;
		break;
		default:
		break;
		}
	break;
	case 0xe1:
		in(IN_BUFFER);
		in(IN_BUFFER);
	break;
	case SHIFT_L|BREAK:
		s_shift_l = FALSE;
	break;
	case SHIFT_R|BREAK:
		s_shift_r = FALSE;
	break;
	default:
		if(ismake(scan_code)){
			buf_write(scan_code);
		}
	break;
	}

	eoi_m();
}

char get_char()
{
	char temp = 0;
	if(s_kb_buf.p_head != s_kb_buf.p_tail)
	{
		temp = *s_kb_buf.p_head;
		++s_kb_buf.p_head;
		if(s_kb_buf.p_head == s_kb_buf.buf+KB_IN_BYTES){
			s_kb_buf.p_head = s_kb_buf.buf;
		}
	}
	return temp;
}
