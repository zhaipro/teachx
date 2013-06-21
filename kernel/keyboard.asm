
%include "..\include\system.inc"

extern	_do_int_key
extern	_save
extern	_restart
global	_int_key

[SECTION .text]
[BITS 32]

; IRQ1
_int_key:
	sub	esp, 4
	call	_save
	add	esp, 4
	call	_do_int_key
	call	_restart
