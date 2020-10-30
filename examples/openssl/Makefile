
all: block epoll async

block:
	gcc -Wall -c block.c -I /root/OUTPUT_openssl_bare/include/
	gcc -o block -L /root/OUTPUT_openssl_bare/lib/ -Wl,-rpath=/root/OUTPUT_openssl_bare/ -lssl -lcrypto block.o

epoll:
	gcc -g -O0 -Wall -c epoll.c -I /root/OUTPUT_openssl_bare/include/
	gcc -g -O0 -o epoll -L /root/OUTPUT_openssl_bare/lib/ -Wl,-rpath=/root/OUTPUT_openssl_bare/ -lssl -lcrypto epoll.o

async:
	gcc -g -O0 -Wall -c async.c -I /root/OUTPUT_openssl_bare/include/
	gcc -g -O0 -o async -L /root/OUTPUT_openssl_bare/lib/ -Wl,-rpath=/root/OUTPUT_openssl_bare/ -lssl -lcrypto async.o

clean:
	@rm -f *.o block epoll async
