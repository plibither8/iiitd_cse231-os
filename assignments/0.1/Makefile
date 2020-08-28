# Generate the preprocessed, intermediate file
preprocess:
	gcc -E hello.c -o hello.i

# Stop after compilation to assembly code
compile:
	gcc -S hello.c -o hello.s

# Assemble the code after compilation and return an object file
assemble:
	gcc -c hello.c -o hello.o

# Link the object to file to create an executable
link:
	gcc hello.c -o hello
