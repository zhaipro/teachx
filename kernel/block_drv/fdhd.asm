
extern	save_restart
extern	_do_floppy
extern	_do_hd
global	_floppy_int
global	_hd_int

[SECTION .text]
[BITS 32]

_floppy_int:
	sub	esp, 4
	push	dword[_do_floppy]
	jmp	save_restart

_hd_int:
	sub	esp, 4
	push	dword[_do_hd]
	jmp	save_restart
