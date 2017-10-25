
APR_SDK=/home/tong/usr/include/apr-1
APR_SDK=/home/tong/usr/

APR_INC=$(APR_SDK)/include/apr-1
APR_LIB=$(APR_SDK)/lib/

CFLAGS+= -I $(APR_INC)

#LDFLAGS+= -L $(APR_LIB) -lapr-1
LDFLAGS += -lpthread

all:
	gcc $(CFLAGS) -g -Wall -c test.c
	gcc $(CFLAGS) $(LDFLAGS) -g -Wall -o test test.o $(APR_LIB)/libapr-1.a
#	gcc $(CFLAGS) $(LDFLAGS) -g -Wall -o test test.o


clean:
	rm -f test test.o
