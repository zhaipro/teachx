
; 因为我们会尽可能少的使用汇编，所以会产生大量的小段汇编代码
; 将它们分别管理在不同文件中确实有点麻烦了
; 因此每个模块只含有一个与模块名同名的汇编文件，用于存放所有相关汇编代码

[SECTION .text]
[BITS 32]

extern save_restart
extern _kernel_start

; 这一段代码必须在最前面
; 其用于跳转到内核初始化代码
push	0
push	_kernel_start
jmp	save_restart


; IPC 模块
global	_ipc_int
extern	_do_ipc		; 响应中断的C函数
extern	save_restart

; 中断函数
_ipc_int:
	push	0
	push	_do_ipc
	jmp	save_restart


; keyboard 模块
extern	_do_int_key
extern	save_restart
global	_int_key

; IRQ1
_int_key:
	push	0
	push	_do_int_key
	jmp	save_restart


; 时钟模块
global	_clock_int
extern	_do_clock_int
extern	_do_sched_clock_int

do_clock_int:
	call	_do_sched_clock_int
	call	_do_clock_int
;	out(INT_M_CTRL,0X20)
	ret

_clock_int:
	sub	esp, 4
	push	do_clock_int
	jmp	save_restart
