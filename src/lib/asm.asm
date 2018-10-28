
; 用于辅助C语言，因为我讨厌使用内嵌汇编代码。
; typedef uint16_t port_t;
; s -> save
; l -> load

global _in
global _out
global _hlt
global _lcr3
global _scr0
global _lcr0
global _jmp
global _lidt
global _sgdt
global _scr2
global _nop
global _sti
global _seflags
global _iret
global _lxs

[SECTION .text]
[BITS 32]

; 读出指定端口
; uint8_t in(port_t port);
_in:
    mov edx, [esp + 4]
    xor eax, eax
    in  al, dx
    ret

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

; void jmp(uint32_t addr);
_jmp:
    jmp [esp + 4]

; void lidt(struct idtptr_t *idtptr)
_lidt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

_sgdt:
    mov eax, [esp + 4]
    sgdt [eax]
    ret

; uint32_t scr2();
_scr2:
    mov eax, cr2
    ret

_nop:
    nop
    ret

_sti:
    sti
    ret

_seflags:
    pushf
    pop eax
    ret

_iret:
    mov esp, [esp + 4]
    iret

; load ds, es, fs, gs
_lxs:
    mov eax, [esp + 4]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret
