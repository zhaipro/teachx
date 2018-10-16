
; 这里只允许定义一些常量
BaseOfOS    equ 08000h  ; 系统被加载到的段地址 (偏移地址为零)
SizeOfOS    equ 3       ; 系统的大小 (单位: 扇区)
PhyOfOS     equ BaseOfOS << 4   ; os.bin 被加载到的物理地址
