
extern _do_page_fault
extern save_restart
global _page_fault

[SECTION .text]
[BITS 32]

_page_fault:
	push	_do_page_fault	
	jmp	save_restart
