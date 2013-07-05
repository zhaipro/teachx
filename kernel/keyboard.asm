
extern	_do_int_key
extern	save_restart
global	_int_key

[SECTION .text]
[BITS 32]

; IRQ1
_int_key:
	push	0
	push	_do_int_key
	jmp	save_restart
