
; 功能：
; 显示Setup ...
; 关中断
; 加载临时gdt
; 开启A20地址总线
; 进入保护模式
; 跳进setup32部分


%include "pm.inc"	; 关于保护模式的常量、宏、以及一些说明
%include "load.inc"	; 关于该模块被加载到内存的情况

org OffsetOfSetup

BaseOfStack:
LABEL_START_CODE:	; 整个代码的其实标号，栈顶就从这里开始

	jmp LABEL_START

[SECTION .gdt]
ALIGN 8

GdtPtr  dw  GdtLen				; 段边界
        dd  BaseOfSetupPhyAddr + LABEL_GDT	; 基地址

ALIGN 8

;				    段基址,  段界限, 属性
; 空描述符
LABEL_GDT:              Descriptor       0,       0, 0
; 0 ~ 4G 的可执行段
LABEL_DESC_FLAT_C:      Descriptor       0, 0fffffh, DA_CR|DA_32|DA_LIMIT_4K
; 0 ~ 4G 的可读写段
LABEL_DESC_FLAT_RW:     Descriptor       0, 0fffffh, DA_DRW|DA_32|DA_LIMIT_4K

GdtLen	equ $ -LABEL_GDT


; 选择子
SelectorFlatC   equ LABEL_DESC_FLAT_C   - LABEL_GDT
SelectorFlatRW  equ LABEL_DESC_FLAT_RW  - LABEL_GDT

[SECTION .data]
Message	db	"Setup ..." ,0dh,0ah,00

[SECTION .s16]
[BITS 16]
LABEL_START:
	mov	ax, cs
	mov	ds, ax
	
	mov	si, Message
	call	DisplayStr

	cli
	lgdt [GdtPtr]

	in  al, 92h
	or  al, 00000010b
	out 92h, al
	
	mov eax, cr0
	or  eax, 1
	mov cr0, eax

	mov	ax, SelectorFlatRW	; 只设置这三个段寄存器即可 
	mov	ds, ax
	mov ss, ax 
	mov	esp, BaseOfStack + BaseOfSetupPhyAddr
	
	jmp	dword SelectorFlatC:(BaseOfSetupPhyAddr+OffsetOfSetup32)

;-------------------------------------------------------------------	
;函数描述：显示字符串,字符串以'00h'结尾
;参数说明: ds:si 指向字符串首地址
;返回值: 无 
DisplayStr:
	pushf
	push ax
	cld				;字符串操作向高地址
	
	mov  ah, 0eh
CONTINE:	
	lodsb
	cmp al,0
	jz  EXIT 
	int 10h
	jmp CONTINE
EXIT:
	pop ax
	popf
	ret
