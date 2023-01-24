CC = gcc
CFLAGS = -g -Wall -std=gnu99
TARGETS = bytecheck

all: $(TARGETS)

bytecheck: bytecheck.c
	$(CC) $(CFLAGS) -o bytecheck bytecheck.c

clean:
	rm *.o 