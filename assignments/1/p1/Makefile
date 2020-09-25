CC = gcc

all: preprocess compile assemble link

# Generate the preprocessed, intermediate file
preprocess: main.c
	$(CC) -E main.c -o main.i

# Stop after compilation to assembly code
compile: main.i
	$(CC) -S main.i -o main.s

# Assemble C code after compilation and return an object file
assemble: main.s
	$(CC) -c main.s -o main.o

# Link the object files to create an executable
link: main.o
	$(CC) main.o -o main

# Clean the processed files to start over
clean:
	rm main main.s main.o main.i

.PHONY: clean
