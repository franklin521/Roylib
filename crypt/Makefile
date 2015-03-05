#makefile for sub-Roylib crypt

#CC=gcc
#CFLAGS = -c -fpic

SRC := base64.c
#OBJS=base64.o
OBJS := $(SRC:.c=.o)

all: $(OBJS)

#$(OBJS): %.o: %.c
.c.a:
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(OBJS)
