
NASM = D:\nasm\nasm.exe
CC = gcc
CFLAGS = -I../include


all : setup32.bin boot.bin setup16.bin

# Á¬½Ósetup
setup32.bin : setup32_asm.o setup32.o ../kernel/asm.o
	coff -Ttext 0x80300 -o setup32.bin setup32_asm.o setup32.o ..\kernel\asm.o

setup32.o : setup32.c

boot.bin : boot.asm
	$(NASM) boot.asm -o boot.bin

setup16.bin : setup16.asm
	$(NASM) setup16.asm -o setup16.bin

setup32_asm.o : setup32.asm
	$(NASM) -f coff -o setup32_asm.o setup32.asm
