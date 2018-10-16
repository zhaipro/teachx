
; setup.asm 负责进入保护模式

%include "include/load.inc"

; now we want to move to protected mode ...
mov ax, BaseOfOS    ; right, forgot this at first. didn't work :-)
mov ds, ax

mov si, Message
call print

cli                 ; no interrupts allowed !

; first we load the segment descriptors
lgdt [gdt_48]       ; load gdt with whatever appropriate

; that was painless, now we enable A20
in  al, 92h         ; open A20 line(Fast Gate A20).
or  al, 00000010b
out 92h, al

; 准备切换到保护模式
mov eax, cr0
or  eax, 1
mov cr0, eax

mov ax, 16
mov ds, ax
mov es, ax
mov fs, ax
mov ss, ax

mov esp, PhyOfOS

; 正式进入保护模式
                    ; segment-descriptor        (INDEX:TI:RPL)
sel_cs0 equ 0x0008  ; select for code segment 0 (  001:0 :00)
; setup 到此结束了吗? No, 是汇编到此结束!
jmp dword sel_cs0:(PhyOfOS + 512)

%include "utils.asm"

gdt:
    dw 0, 0, 0, 0   ; dummy

    dw 0x07FF       ; 8Mb - limit=2047 (2048*4096=8Mb)
    dw 0x0000       ; base address=0
    dw 0x9A00       ; code read/exec
    dw 0x00C0       ; granularity=4096, 386

    dw 0x07FF       ; 8Mb - limit=2047 (2048*4096=8Mb)
    dw 0x0000       ; base address=0
    dw 0x9200       ; data read/write
    dw 0x00C0       ; granularity=4096, 386

gdt_48:
    dw 0x800            ; gdt limit=2048, 256 GDT entries
    dd PhyOfOS + gdt    ; gdt base
    ; PhyOfOS + gdt is the real gdt

Message db "Setup ...", 00
