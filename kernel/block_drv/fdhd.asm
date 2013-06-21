
extern	_save
extern	_restart
extern	_do_floppy
extern	_do_hd
global	_floppy_int
global	_hd_int

[SECTION .text]
[BITS 32]

_floppy_int:
	sub  esp, 4
	call _save
	add  esp, 4
	call [_do_floppy]
	call _restart

_hd_int:	
	sub  esp, 4
	call _save
	add  esp, 4
	call [_do_hd]
	call _restart
