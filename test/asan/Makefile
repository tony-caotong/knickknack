all:
	gcc -c test.c -fsanitize=address -fno-omit-frame-pointer
	gcc -o test  -fsanitize=address -fno-omit-frame-pointer -lasan test.o 

clean:
	rm -f test test.o
