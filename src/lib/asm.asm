
; 用于辅助C语言，因为我讨厌使用内嵌汇编代码。
; typedef uint16_t port_t;
; s -> save
; l -> load

global _out
global _hlt
global _lcr3
global _scr0
global _lcr0

[SECTION .text]
[BITS 32]

; 写入指定端口
; void out(port_t port, uint8_t value);
_out:
    mov dx, [esp + 4]   ; port
    mov al, [esp + 8]   ; value
    out dx, al
    ret

; 停机
_hlt:
    hlt
    ret

; void lcr3(uint32_t page_directory);
_lcr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

_scr0:
    mov eax, cr0
    ret

_lcr0:
    mov eax, [esp + 4]
    mov cr0, eax
    ret
