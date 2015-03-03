#makefile for Roylib
CC=gcc
CFLAGS= -c -fpic
CCFLAGS=
SUFFIX=.so

export CC
export CFLAGS

#export all variables
#export

SRC := util.c misc.c stack.c red_black_tree.c

#OBJS= util.o misc.o stack.o red_black_tree.o
OBJS := $(SRC:.c=.o)

TARGET=libRoylib$(SUFFIX)

all: target crypt

.PHONY: all crypt


target: $(OBJS)
	$(CC) -shared -o $(TARGET) $(OBJS)

$(OBJS): %.o: %.c 
#	@echo compiling $< ...
	$(CC) $(CFLAGS) -o $@ $<

crypt: 
	@cd ./crypt && make


.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)
	@cd ./crypt && make clean
