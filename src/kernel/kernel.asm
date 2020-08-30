
global _ipc
extern _do_ipc

[SECTION .text]
[BITS 32]

_ipc:
    call    _do_ipc
    iret
