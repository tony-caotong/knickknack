CC=gcc
CFLAGS= \
	-Wall -g -O0 \
	-I sdk/include

LDFLAGS= \

#	-L sdk/lib/ -lmsgpackc
#	-L sdk/lib/ -l:libmsgpackc.a

LDLIBS = \
	sdk/lib/libmsgpackc.a


TARGET=msgpack_test

$(TARGET): test.o
	$(CC) $(LDFLAGS) -o $@ $< $(LOADLIBES) $(LDLIBS)

.PHONY: clean TAGS
clean:
	@rm -f $(TARGET) *.o

TAGS:
	@ctags -R ./ --c-kinds=+p --exclude='sdk/*'
