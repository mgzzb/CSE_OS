CC = gcc
CFLAGS = -Wall
TARGETS = chime

all: $(TARGETS)

chime: chime.c
	$(CC) $(CFLAGS) -o chime chime.c -lpthread

clean:
	rm -rf *.o chime
