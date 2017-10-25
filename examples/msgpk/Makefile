all:
	gcc -I ../../../../../../INSTALL/include -c test.c
	gcc -o test test.o -L ../../../../../../INSTALL/lib -lmsgpackc -Wl,--rpath=../../../../../../INSTALL/lib

clean:
	rm -rf test.o test
