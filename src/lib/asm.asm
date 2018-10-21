
; 用于辅助C语言，因为我讨厌使用内嵌汇编代码。
; typedef uint16_t port_t;

global _out
global _hlt

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
