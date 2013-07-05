
%include "system.inc"

extern save_restart

extern	_do_divide_error
global	_divide_error

extern	_do_int3
global	_debug
global	_int3

extern	_do_nmi
global	_nmi

extern	_do_overflow
global	_overflow

extern	_do_bounds
global	_bounds

extern	_do_invalid_op
global	_invalid_op

extern	_do_double_fault
global	_double_fault

extern	_do_coprocessor_segment_overrun
global	_coprocessor_segment_overrun

extern	_do_invalid_TSS
global	_invalid_TSS

extern	_do_segment_not_present
global	_segment_not_present

extern	_do_stack_segment
global	_stack_segment

extern	_do_general_protection
global	_general_protection

extern	_do_unknow_int
global	_unknow_int


[SECTION .text]
[BITS 32]

; int0
_divide_error:
	push	0
	push	_do_divide_error
	jmp	save_restart

; int1
_debug:
	push	1
	push	_do_int3
	jmp	save_restart

; int2
_nmi:
	push	2
	push	_do_nmi
	jmp	save_restart

; int3
_int3:
	push	3
	push	_do_int3
	jmp	save_restart

; int4
_overflow:
	push	4
	push	_do_overflow
	jmp	save_restart

; int5
_bounds:
	push	5
	push	_do_bounds
	jmp	save_restart

; int6
_invalid_op:
	push	6
	push	_do_invalid_op
	jmp	save_restart

; int7	协处理器不存在，在其他文件中

; int8	
_double_fault:
	push	_do_double_fault
	jmp	save_restart

; int9
_coprocessor_segment_overrun:
	push	9
	push	_do_coprocessor_segment_overrun
	jmp	save_restart
; int10
_invalid_TSS:
	push	_do_invalid_TSS
	jmp	save_restart

; int11
_segment_not_present:
	push	_do_segment_not_present
	jmp	save_restart

; int12
_stack_segment:
	push	_do_stack_segment
	jmp	save_restart

; int13
_general_protection:
	push	_do_general_protection
	jmp	save_restart

; int14	页错误	在内存管理中

; void unknow_int();
_unknow_int:
	push	0
	push	_do_unknow_int
	jmp save_restart
