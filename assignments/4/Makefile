CC = gcc
CC_FLAGS = -pthread

compile1:
	$(CC) $(CC_FLAGS) blocking_2019061.c -o blocking.o

compile2:
	$(CC) $(CC_FLAGS) nonblocking_2019061.c -o nonblocking.o

clean:
	rm *.o

.PHONY: clean
