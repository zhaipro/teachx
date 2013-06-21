
; 用于辅助C语言，因为我讨厌使用内嵌汇编代码。
; typedef char u8;
; typedef int port_t;
; s -> save
; l -> load

global	_cli
global	_enable_pagine
global	_hlt
global	_in
global	_lcr2
global	_lcr3
global	_lflags
global	_lgdt
global	_lidt
global	_ltr
global	_nop
global	_out
global	_port_readw
global	_port_writew
global	_scr2
global	_scr3
global	_sflags
global	_sti

[SECTION .text]
[BITS 32]

; ------------------------------------------------------------------------
; 写入指定端口
; void out(port_t port, u8 value);
; ------------------------------------------------------------------------
_out:
	mov	edx, [esp + 4]		; port
	mov	al, [esp + 8]		; value
	out	dx, al
	ret

; ------------------------------------------------------------------------
; 读出指定端口
; u8 in(port_t port);
; ------------------------------------------------------------------------
_in:
	mov	edx, [esp + 4]		; port
	xor	eax, eax
	in	al, dx
	ret

_cli:
	cli
	ret

_sti:
	sti
	ret

; lgdt(GDTPTR *GdtPtr)
_lgdt:
	mov	eax, [esp + 4]
	lgdt	[eax]
	ret

; lidt(IDTPTR *IdtPtr)
_lidt:
	mov	eax, [esp + 4]
	lidt	[eax]
	ret

; ltr(u32 selector);
_ltr:
	ltr	[esp + 4]
	ret

_lcr2:
	mov	eax, [esp + 4]
	mov	cr2, eax
	ret

_scr2:
	mov	eax, cr2
	ret

; lcr3(u32 pPageDir)
_lcr3:
	mov	eax, [esp + 4]
	mov	cr3, eax
	ret

_scr3:
	mov	eax, cr3
	ret

_enable_pagine:
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:	nop
	ret

_nop:
	nop
	ret

; port,buf,nr
_port_readw:
	mov	edx, [esp + 4]
	mov	edi, [esp + 8]
	mov	ecx, [esp + 12]
	rep
	insw
	ret

_port_writew:
	mov	edx, [esp + 4]
	mov	esi, [esp + 8]
	mov	ecx, [esp + 12]
	rep
	outsw
	ret

_hlt:
	hlt
	ret

_sflags:
	pushf
	pop	eax
	ret

_lflags:
	push	dword [esp + 4]
	popf
	ret
