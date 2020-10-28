all:
	gcc -Wall -c main.c -I /root/OUTPUT_openssl_bare/include/
	gcc -o test -L /root/OUTPUT_openssl_bare/lib/ -Wl,-rpath=/root/OUTPUT_openssl_bare/ -lssl -lcrypto main.o
clean:
	@rm -f main.o test
