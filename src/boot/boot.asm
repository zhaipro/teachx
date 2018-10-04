
org 07c00h                  ; 告诉编译器程序加载到7c00处

call Clear
mov si, BootMessage
call print                  ; 调用显示字符串例程
jmp $                       ; 无限循环

%include "utils.asm"

; 清屏
Clear:
    mov ax, 3
    int 10h
    ret

BootMessage     db "Hello teachx world!", 00

times 510 - ($ - $$) db 0   ; 填充剩下的空间, 使生成的二进制代码恰好为512字节
dw 0xaa55                   ; 结束标志
