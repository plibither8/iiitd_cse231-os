CC = gcc
ASM = nasm
ASM_FLAGS = -felf64

# Generate the preprocessed, intermediate file
preprocess: prog-add.c add.asm
	$(CC) -E prog-add.c -o prog-add.i
	$(ASM) $(ASM_FLAGS) -E add.asm -o add.i

# Stop after compilation to assembly code
compile: prog-add.c prog-add.i
	$(CC) -S prog-add.i -o prog-add.s

# Assemble C code after compilation and return an object file
# Assemble assembly file with nasm
assemble: prog-add.c add.asm
	$(CC) -c prog-add.c -o prog-add.o
	$(ASM) $(ASM_FLAGS) add.asm -i add.o

# Link the object files to create an executable
link: prog-add.o add.o
	$(CC) prog-add.o add.o -o prog-add

# Clean the processed files to start over
clean:
	rm prog-add prog-add.s prog-add.o prog-add.i add.i add.o

.PHONY: clean
