CC = gcc
CC_FLAGS = -c
ASM = nasm
ASM_FLAGS = -felf64
LINK = ld
LINK_FLAGS = -e main

compile: p1.asm p1.c
	$(ASM) $(ASM_FLAGS) p1.asm -o p1_asm.o
	$(CC) $(CC_FLAGS) p1.c -o p1.o

link: p1.asm p1.c
	$(LINK) p1.o p1_asm.o -o p1

link_main: p1.asm p1.c
	$(LINK) $(LINK_FLAGS) p1.o p1_asm.o -o p1

# Clean the processed files to start over
clean:
	rm p1 *.o

.PHONY: clean
