CC = gcc
CC_FLAGS = -o
ODIR = bin

all: prepare p8sh cat rm ls mkdir date

prepare:
	mkdir -p bin

p8sh: p8sh.c
	$(CC) $(CC_FLAGS) p8sh p8sh.c -lm -g

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
