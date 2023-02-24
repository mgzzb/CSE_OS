CC = gcc
CFLAGS = -Wall
TARGETS = chime

all: $(TARGETS)

chime : chime.c
	gcc -o chime -Wall --std=c99 chime.c -lpthread


clean:
	rm -rf *.o chime
