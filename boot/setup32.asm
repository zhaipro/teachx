
; 从此以后的代码在保护模式下执行
; 32 位代码段. 由实模式跳入
; 这里这是一个跳板，因为我的连接器的限制，它需要一个头程序段。

%include "..\include\system.inc"

extern _setup32
[SECTION .text]
[BITS	32]

LABEL_PM_START:
	call	_setup32	; 这个函数只负责初始化
	
	mov	ax, SELECTOR_KERNEL_DS
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, TOP_STACK
	mov	ax, SELECTOR_GS
	mov	gs, ax

	jmp	SELECTOR_KERNEL_CS:OFFSET_KERNEL	; 系统可执行段开头
