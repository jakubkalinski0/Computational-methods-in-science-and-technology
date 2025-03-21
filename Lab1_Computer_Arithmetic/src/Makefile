# Makefile of the project
CC = gcc
CFLAGS = -Wall -Wextra -O2 -lm

all: program

program: main.o functions.o utilities.o
	$(CC) $(CFLAGS) -o program main.o functions.o utilities.o -lm

main.o: main.c functions.h utilities.h
	$(CC) $(CFLAGS) -c main.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -f *.o program *.csv *.png *.gp

run: program
	./program