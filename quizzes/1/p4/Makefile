CC = gcc
CC_FLAGS = -m32
ASM = nasm
ASM_FLAGS_32 = -felf32
ASM_FLAGS_64 = -felf64

long_add: P4_1.asm P4.c
	$(ASM) $(ASM_FLAGS_32) P4_1.asm -o P4_1.o
	$(CC) $(CC_FLAGS) P4.c P4_1.o -o P4
	./P4

extended_add: P4_2.asm P4.c
	$(ASM) $(ASM_FLAGS_64) P4_2.asm -o P4_2.o
	$(CC) P4.c P4_2.o -o P4
	./P4

# Clean the processed files to start over
clean:
	rm P4 *.o

.PHONY: clean

