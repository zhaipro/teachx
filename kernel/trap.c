
#include "kernel.h"
#include "stdio.h"

//初始化代码将把它们设置到idt中 
void divide_error();				//0
void debug();						//1
void nmi();							//2 
void int3();						//3
void overflow();					//4
void bounds();						//5
void invalid_op();					//6

void double_fault();				//8
void coprocessor_segment_overrun();	//9
void invalid_TSS();					//10
void segment_not_present();			//11
void stack_segment();				//12
void general_protection();			//13

void unknow_int();

void init_trap()
{
	int i;
	
	set_sys_idt( 0,divide_error);
	set_sys_idt( 1,debug);
	set_sys_idt( 2,nmi);
	set_sys_idt( 3,int3);
	set_sys_idt( 4,overflow);
	set_sys_idt( 5,bounds);
	set_sys_idt( 6,invalid_op);
	
	set_sys_idt( 7,unknow_int);
	
	set_sys_idt( 8,double_fault);
	set_sys_idt( 9,coprocessor_segment_overrun);
	set_sys_idt(10,invalid_TSS);
	set_sys_idt(11,segment_not_present);
	set_sys_idt(12,stack_segment);
	set_sys_idt(13,general_protection);
	for(i=14;i<256;i++)
		set_sys_idt(i,unknow_int);
}


void do_divide_error()
{
	printf("#0 de");
}

void do_debug()
{
	printf("#1 db");
}

void do_nmi()
{
	printf("#2");
}

void do_int3()
{
	printf("#3 bp");
}

void do_overflow()
{
	printf("#4 of");
}

void do_bounds()
{
	printf("#5 br");
}

void do_invalid_op()
{
	printf("#6 ud");
}

void do_double_fault()
{
	printf("#8 df");
}

void do_coprocessor_segment_overrun()
{
	printf("#9");
}

void do_invalid_TSS()
{
	printf("#10 ts");
}

void do_segment_not_present()
{
	printf("#11 np");
}

void do_stack_segment()
{
	printf("#12 ss");
}

void do_general_protection()
{
	printf("#13 gp");
}

void do_unknow_int()
{
	printf("unknow int\n");
}
