
%include "system.inc"
%include "process.inc"

[SECTION .data]
[BITS 32]
int_count DD 0	;中断嵌套的次数 

[SECTION .text]
[BITS 32]

global save_restart
global _restart
extern _do_iret
extern _switch_to


; 首先压入错误码
; 然后压入回调函数 
; 再然后使用jmp跳到该函数
save_restart:
	pushad
	push	fs
	push	ds
	mov	eax, esp
	mov	ebx, [esp + 10*4]	; CALL
	mov	ecx, [esp + 11*4]	; ERROR_CODE
	cmp	dword[int_count], 0
	jne	.1
	mov	dx, SELECTOR_KERNEL_DS
	mov	ds, dx
	mov	es, dx
	mov	esp, TOP_STACK
.1:
	inc	dword[int_count]
	push	eax
	push	ecx			; push error_code
	call	ebx
	add	esp, 4			; pop error_code in kernel stack
	dec	dword[int_count]
	cmp	dword[int_count], 0
	jne	.2
	call	_do_iret
	push	eax
	call	_switch_to
.2:
	call	_restart
 
	
;设置esp指向STACK_FRAME结构对象，注意这里不切换cr3
_restart:
	mov	esp, [esp + 4]
	pop	ds
	mov	ax,ds
	mov	es,ax 
	pop	fs
	popad 
	add	esp, 8	; 跳过return_addr,error_code
	iret
