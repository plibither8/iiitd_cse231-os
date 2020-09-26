CC = gcc
CC_FLAGS = -o
ODIR = bin

all: prepare mihish cat rm ls mkdir date

prepare:
	mkdir -p bin

mihish: mihish.c
	$(CC) $(CC_FLAGS) mihish mihish.c

cat: cat.c
	$(CC) $(CC_FLAGS) $(ODIR)/cat cat.c -lm

rm: rm.c
	$(CC) $(CC_FLAGS) $(ODIR)/rm rm.c

ls: ls.c
	$(CC) $(CC_FLAGS) $(ODIR)/ls ls.c

mkdir: mkdir.c
	$(CC) $(CC_FLAGS) $(ODIR)/mkdir mkdir.c

date: date.c
	$(CC) $(CC_FLAGS) $(ODIR)/date date.c
