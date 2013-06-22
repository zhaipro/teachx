
; 这里只有一些常量定义

BaseOfSetup		equ	 08000h	; SETUP.BIN 被加载到的位置 ----  段地址
OffsetOfSetup		equ	  0100h	; SETUP.BIN 被加载到的位置 ----  偏移地址
OffsetOfSetup16		equ	  0100h ; SETUP16.BIN 在内存中的偏移
OffsetOfSetup32		equ	  0300h ; SETUP32.BIN 在内存中的偏移

BaseOfSetupPhyAddr	equ	BaseOfSetup * 10h	; SETUP.BIN 被加载到的位置 ---- 物理地址 (= BaseOfSetup * 10h)
