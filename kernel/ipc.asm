
[SECTION .text]
[BITS 32]

global	_ipc_int
extern	_do_ipc		; 响应中断的C函数
extern	save_restart

; 中断函数
_ipc_int:
	push	0
	push	_do_ipc
	jmp	save_restart
