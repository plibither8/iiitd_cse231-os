# assemble assembly file with nasm
assemble:
	nasm -felf64 add.asm -i add.o

# compile C code and link with assembly output
compile_and_link:
	gcc prog-add.c add.o -i prog-add
