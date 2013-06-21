
%include "..\include\system.inc"
%include "..\include\process.inc"

[SECTION .data]
[BITS 32]
int_count DD 0	;中断嵌套的次数 

[SECTION .text]
[BITS 32]

extern _schedule

; 使用jmp跳到该函数
; 首先压入错误码
; 再次压入回调函数 
save_restart:
	pushad
	push	fs
	push	ds
	mov	dx, ds
	mov	eax, esp
	mov	ebx, [esp + 10*4]	; CALL
	mov	ecx, [esp + 11*4]	; ERROR_CODE
	inc	dword[int_count]
	cmp	dx, SELECTOR_KERNEL_DS
	je	.1
	mov	dx, SELECTOR_KERNEL_DS
	mov	ds, dx
	mov	esp, TOP_STACK
.1:
	push	eax
	push	ecx			; push error_code
	call	ebx
	add	esp, 4			; pop error_code in kernel stack
	dec	dword[int_count]
	cmp	dword[int_count], 0
	jne	.2
	call	_schedule
.2:
	call	_restart
 
	
;设置esp指向STACK_FRAME结构对象，注意这里不切换cr3
_restart:
	mov	esp, [esp + 4]
	pop	ds
	pop	fs
	popad 
	add	esp, 8	; 跳过return_addr,error_code
	iret

global	_clock_int
extern	_do_clock_int

_clock_int:
	sub	esp, 4
	push	_do_clock_int
	jmp	save_restart


global	_ipc_int
extern	_do_ipc		; 响应中断的C函数

; 中断函数
_ipc_int:
	push	ds
	push	fs
	mov	ebp, esp
	mov	ax, ds
	cmp	ax, SELECTOR_KERNEL_DS
	je	.3
	mov	ax, SELECTOR_KERNEL_DS	; 首次进入内核态
	mov	ds, ax
	mov	esp, TOP_STACK
.3:
	push	ebp					; 保存这个堆栈指针是为了返回
	push	edx
	push	ecx
	push	ebx
	call	_do_ipc
	add	esp, 3*4
	pop	esp
	pop	ds
	iret
