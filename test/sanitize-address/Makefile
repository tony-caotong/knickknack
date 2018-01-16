CC=clang

all:
	$(CC) -Wall -fsanitize=address -g -c main.c
	#gcc -Wall -c main.c
	$(CC) -Wall -lasan main.o -o test

clean:
	rm -f test main.o
