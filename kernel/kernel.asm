
[SECTION .text]
[BITS 32]

extern save_restart
extern _kernel_start

push	0
push	_kernel_start
jmp	save_restart
