# Makefile dla projektu
CC = gcc
CFLAGS = -Wall -Wextra -O2 -lm

all: program

program: main.o funkcje.o narzedzia.o
	$(CC) $(CFLAGS) -o program main.o funkcje.o narzedzia.o -lm

main.o: main.c funkcje.h narzedzia.h
	$(CC) $(CFLAGS) -c main.c

funkcje.o: funkcje.c funkcje.h
	$(CC) $(CFLAGS) -c funkcje.c

narzedzia.o: narzedzia.c narzedzia.h
	$(CC) $(CFLAGS) -c narzedzia.c

clean:
	rm -f *.o program *.csv *.png *.gp

run: program
	./program