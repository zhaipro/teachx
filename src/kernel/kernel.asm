
global _ipc
extern _do_ipc

global _timer_int
extern _do_timer_int

[SECTION .text]
[BITS 32]

_ipc:
    pushad
    call    _do_ipc
    mov     [esp + 7 * 4], eax
    popad
    iret

_timer_int:
    pushad
    call    _do_timer_int
    popad
    iret
