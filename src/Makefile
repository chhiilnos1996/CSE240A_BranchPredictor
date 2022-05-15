CC=gcc
OPTS=-g -std=c99 -Werror

all: main.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o -lm

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c -lm

predictor.o: predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c -lm

clean:
	rm -f *.o predictor;
