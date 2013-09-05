
global _ipc
global __pthread_getspecific
global __pthread_setspecific

[SECTION .text]
[BITS 32]

; 用户接口
_ipc:
	mov	ebx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	edx, [esp + 12]
	int	0x80
	ret

__pthread_getspecific:
	mov eax, [esp + 4]
	mov	eax, [fs:eax]
	ret

__pthread_setspecific:
	mov	ebx, [esp + 4]
	mov eax, [esp + 8]
	mov [fs:ebx], eax
	ret
