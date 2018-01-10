all:
	gcc -Wall -fsanitize=address -g -c main.c
	#gcc -Wall -c main.c
	gcc -Wall -fsanitize=address main.o -o test

clean:
	rm -f test main.o
