
newline db 0dh, 0ah, 00

; 显示字符串, 字符串以'00h'结尾
; 参数: ds:si 指向字符串首地址
_print:
    mov  ah, 0eh
CONTINE:
    lodsb
    cmp al, 0
    jz  EXIT
    int 10h
    jmp CONTINE
EXIT:
    ret

print:
    call _print
    mov si, newline
    call _print
    ret
