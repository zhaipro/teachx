
%include "include/load.inc"

mov ax, BaseOfOS
mov ds, ax
mov si, Message
call print

jmp $

%include "utils.asm"

Message db "Setup ...", 00
