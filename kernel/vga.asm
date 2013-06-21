
[SECTION .text]
[BITS 32]

global	_vga_write
global	_vga_write_color


_vga_write_color:
	mov	edi, [esp + 4]	; addr
	mov	esi, [esp + 8]	; str
	mov	eax, [esp + 12]	; color
	jmp	for
_vga_write:
	mov	edi, [esp + 4]	; addr
	mov	esi, [esp + 8]	; str
	mov	ah, 0Fh		; color
for:
	lodsb
	test	al, al
	jz	.2
	cmp	al, 0Ah	; ÊÇ»Ø³µÂð?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	for
.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	for
.2:
	mov	eax, edi
	ret
