CC=gcc

EXTRAFLAGS= -Wall -O0 -g

CFLAGS= $(EXTRAFLAGS) \
	-I /anthropoid/include/
LDFLAGS= $(EXTRAFLAGS) \
	 -L /anthropoid/lib/
LDLIBS= -lconfig

CPPFLAGS=

TARGET=config
OBJ=$(patsubst %.c, %.o, $(wildcard *.c))

$(TARGET): $(OBJ)

.PHONY: clean
clean:
	@rm -f $(TARGET) $(OBJ)
