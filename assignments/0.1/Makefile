CC = gcc

# Generate the preprocessed, intermediate file
preprocess: hello.c
	$(CC) -E hello.c -o hello.i

# Stop after compilation to assembly code
compile: hello.c hello.i
	$(CC) -S hello.i -o hello.s

# Assemble the code after compilation and return an object file
assemble: hello.c
	$(CC) -c hello.c -o hello.o

# Link the object to file to create an executable
link: hello.c hello.o
	$(CC) hello.c -o hello

# Clean the processed files to start over
clean:
	rm hello hello.s hello.o hello.i

.PHONY: clean
