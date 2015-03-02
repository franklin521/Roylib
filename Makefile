#makefile for Roylib
CC=gcc
CFLAGS= -c -fpic
CCFLAGS=
SUFFIX=.so

SRC= util.c misc.c stack.c red_black_tree.c

OBJS= util.o misc.o stack.o red_black_tree.o

TARGET=libRoylib$(SUFFIX)

all: target

.PHONY: all


target: $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS)

$(OBJS): %.o: %.c 
#	@echo compiling $< ...
	$(CC) $(CFLAGS) -o $@ $<


.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
