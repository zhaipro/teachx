
%include "include/load.inc"

org 07c00h                  ; 告诉编译器程序加载到7c00处

call Clear
mov si, BootMessage
call print                  ; 调用显示字符串例程
call LoadOS
mov si, Message
call print
jmp BaseOfOS:0              ; Boot Sector 的使命到此结束

%include "utils.asm"

; 清屏
Clear:
    mov ax, 3
    int 10h
    ret

; 从 A 盘加载操作系统
LoadOS:
    ; https://en.wikipedia.org/wiki/INT_13H
    mov ax, BaseOfOS    ; 先借用 ax 设置 es
    mov es, ax
    mov bx, 0           ; es:bx <- 加载地址
    mov ah, 2           ; 调用读取函数
    mov al, SizeOfOS    ; 读 SizeOfOS 个扇区
    mov ch, 0           ; 柱面号
    mov cl, 2           ; 起始扇区号 (扇区号从 1 开始)
    mov dh, 0           ; 磁头号
    mov dl, 0           ; 驱动器号 (0 表示 A 盘)
    int 13h
    ret

BootMessage     db "Hello teachx world!", 00
Message         db "Boot ready.", 00

times 510 - ($ - $$) db 0   ; 填充剩下的空间, 使生成的二进制代码恰好为512字节
dw 0xaa55                   ; 结束标志
