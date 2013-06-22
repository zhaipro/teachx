
/*
提供最简单的初始化工作 
系统的物理内存布局 
 | 1M |2K |2K |  4K  |  4K  |  4K  |   ?  |
[|保留|IDT|GDT|PT512 |PT513 |STACK |kernel|] 

暂时只给内核4K的栈空间，希望在初始化时够用，应该够用吧！ 
*/
#include "kernel.h"
#include "asm.h"
#include "memory.h"
#include "setup.h"

#define _idt (_1M)
#define _gdt ((struct DESCRIPTOR*)(_1M+_2K))
#define _pt512 ((u32*)(_1M+_2K+_2K))
#define _pt513 ((u32*)(_1M+_2K+_2K+_4K))
#define _stack (_1M+_2K+_2K+_4K+_4K)
#define _knl ((u32*)(_1M+_2K+_2K+_4K+_4K+_4K))
#define _knl_in_setup ((u32*)(0x80700))
#define SIZE_KERNEL	(3*_4K)
#define _temp_pt0 ((u32*)0)

/*
端口0x70用来设置CMOS数据地址
端口0x71用来读写0x70设置的地址中的数据单元字节内容 
*/
static u8 cmos_read(u8 addr)
{
	out(0x70,addr);
	return in(0x71);
}

static void memzero(void *p,size_t count)
{
	while(count--)
	{
		*(char*)p = 0;
		p++;
	}
}

void get_sys_info()
{
	u8 i;
	u32 offset = *(u16*)(0x104);
	u32 seg = *(u16*)(0x106);
	u8 *info = (u8*)((seg<<4) + offset);
	for(i=0;i<16;i++)
		g_sys_info->hd0[i] = info[i];
		
	offset = *(u16*)(0x118);
	seg = *(u16*)(0x11a);
	info = (u8*)((seg<<4) + offset);
	for(i=0;i<16;i++)
		g_sys_info->hd1[i] = info[i];
		
	offset = *(u16*)(0x78);
	seg = *(u16*)(0x7a);
	info = (u8*)((seg<<4) + offset);
	for(i=0;i<11;i++)
		g_sys_info->fd_info[i] = info[i];
	for(i=0;i<0x34;i++)
		g_sys_info->cmos_data[i] = cmos_read(i);
	
	g_sys_info->first_page_addr = (uint)_knl + SIZE_KERNEL;
	
	g_sys_info->last_page_addr = g_sys_info->cmos_data[0x18];
	g_sys_info->last_page_addr <<= 8;
	g_sys_info->last_page_addr |= g_sys_info->cmos_data[0x17];
	g_sys_info->last_page_addr <<= 10;
	g_sys_info->last_page_addr += _1M;
}

static void init_idt()
{
	memzero((void*)_idt,256*sizeof(struct DESCRIPTOR));
}

static void init_gdt()
{
	int i;
// 无效段 
	set_gdt(0,0,0,0);
// VGA段
	set_gdt(1,(void*)VGA,8,DA_DRW|DA_LIMIT_4K|DA_DPL3);
// 系统段，系统段长度日后再说 
	set_gdt(2,(void*)0,0x80520,DA_C|DA_32|DA_LIMIT_4K);	
	set_gdt(3,(void*)0,0x80520,DA_DRW|DA_32|DA_LIMIT_4K);
// 用户段 
	set_gdt(4,0,512*1024,DA_C|DA_32|DA_LIMIT_4K);
	set_gdt(5,0,512*1024,DA_DRW|DA_32|DA_LIMIT_4K);
// TSS段 
	set_gdt(6,0,0,0);							// 有内核对其进行初始化 
// 系统进程段 
	set_gdt(7,0,0x80520,DA_C|DA_32|DA_LIMIT_4K|DA_DPL1);
	set_gdt(8,0,0x80520,DA_DRW|DA_32|DA_LIMIT_4K|DA_DPL1);
	for(i=9;i<256;i++)
		set_gdt(i,0,0,0);
}

static void init_kernel()
{
	u32 i;
	for(i=0;i<SIZE_KERNEL/sizeof(_knl[0]);i++)
		_knl[i] = _knl_in_setup[i];
}

#define PA_P	0X01
#define PA_RW	0X02
#define PA_USER	0X04

static void init_paging()
{
	u32 *p;		//既是页表也是也目录表 
	u32 temp;
//初始化页目录
	memzero(_pt512,sizeof(u32)*1024);
//为了在开启分页后仍然可以继续执行初始化代码 
	_pt512[0]   = ((u32)_temp_pt0) | PA_P | PA_RW;
	_temp_pt0[0]    = 0 | PA_P | PA_RW;			//低地址用于存放系统信息表 
	_temp_pt0[0x80] = 0x80000 | PA_P | PA_RW;
	_temp_pt0[0x81] = 0x81000 | PA_P | PA_RW;
//设置VGA段	
	temp = 0xb8000 | PA_P | PA_RW | PA_USER;
	for(p = _pt513;p!=_pt513 + 8;p++)
	{
		*p = temp;
		temp += (4*1024);
	}
//设置IDT/GDT段 
	*p  = ((u32)_idt) | PA_P | PA_RW;
//设置两个页表
	_pt512[512] = ((u32)_pt512) | PA_P | PA_RW;
	_pt512[513] = ((u32)_pt513) | PA_P | PA_RW;
//设置栈段
	_pt513[264] = _stack | PA_P | PA_RW;
//设置kernel段
	_pt513[265] = ((u32)_knl) | PA_P | PA_RW;
	_pt513[266] = ((u32)_knl+_4K) | PA_P | PA_RW;
	_pt513[267] = ((u32)_knl+2*_4K) | PA_P | PA_RW;
//启动分页机制 
	lcr3((cr3_t)_pt512);
	enable_pagine();
}

void setup32()
{
	struct GDTPTR GdtPtr;
	struct IDTPTR IdtPtr;
	
	get_sys_info();
	init_idt();
	init_kernel();
	init_paging();
	init_gdt();
//启动分页后加载gdt、idt 
	GdtPtr.limit = 256 * sizeof(struct DESCRIPTOR);
	GdtPtr.BaseAddr = gdt;
	lgdt(&GdtPtr);
	IdtPtr.limit = 256 * sizeof(struct DESCRIPTOR);
	IdtPtr.BaseAddr = idt;
	lidt(&IdtPtr);
}
