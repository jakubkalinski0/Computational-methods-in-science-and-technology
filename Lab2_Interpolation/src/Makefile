CC = gcc
CFLAGS = -Wall -lm

all: interpolation

interpolation: main.o interpolation.o visualization.o
	$(CC) -o interpolation main.o interpolation.o visualization.o $(CFLAGS)

main.o: main.c interpolation.h visualization.h
	$(CC) -c main.c $(CFLAGS)

interpolation.o: interpolation.c interpolation.h
	$(CC) -c interpolation.c $(CFLAGS)

visualization.o: visualization.c visualization.h interpolation.h
	$(CC) -c visualization.c $(CFLAGS)

clean:
	rm -f *.o interpolation