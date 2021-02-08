

all:
	gcc -g -O0 -c test.c -fsanitize=address -fno-omit-frame-pointer
	gcc -g -O0 -o test  -fsanitize=address -fno-omit-frame-pointer -lasan test.o 
#	gcc -g -O0 -c test.c
#	gcc -g -O0 -o test test.o 

clang:
	clang -g -O0 -c test.c
	clang -g -O0 -o test test.o 

clang-asan:
	clang -g -O0 -c test.c -fsanitize=address -fno-omit-frame-pointer
	clang -g -O0 -o test -fsanitize=address -fno-omit-frame-pointer test.o 

clean:
	rm -f test test.o
