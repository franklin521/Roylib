#makefile for Roylib
CC=gcc
CXX=g++
CFLAGS= -c -fpic
CCFLAGS= $(CFLAGS)
SUFFIX=.so

export CC
export CFLAGS

#export all variables
#export

SRC := util.c misc.c stack.c red_black_tree.c
CPPSRC := bloom.cpp

#OBJS= util.o misc.o stack.o red_black_tree.o
#OBJS := $(SRC:.c=.o)
OBJS := $(SRC:%.c=%.o)
CPPOBJS := $(CPPSRC:%.cpp=%.o)

TARGET=libRoylib$(SUFFIX)

all: target crypt

.PHONY: all crypt


target: $(OBJS) $(CPPOBJS)
	$(CC) -shared -o $(TARGET) $(OBJS) $(CPPOBJS)

$(OBJS): %.o: %.c 
#.c.a:
#	@echo compiling $< ...
	$(CC) $(CFLAGS) -o $@ $<

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CCFLAGS) -o $@ $<

crypt:
	@cd ./crypt && make


.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS) $(CPPOBJS)
	@cd ./crypt && make clean
