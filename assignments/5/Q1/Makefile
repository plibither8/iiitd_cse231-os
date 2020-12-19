compile:
	nasm -f bin Q1_2010961.asm -o boot.bin

run: compile
	qemu-system-x86_64 -fda boot.bin
