#CC=clang
CC=gcc

CFLAGS=$(shell pkg-config --cflags libhs)
LIBS=$(shell pkg-config --libs --static libhs)

TARGET=test

OBJ = test.o

.PHONY: ENV clean
$(TARGET): $(OBJ) ENV
	$(CC) -o $@ $< $(LIBS) 

ENV:
	@echo 
	@echo "export PKG_CONFIG_PATH=/root/src/thirdparty/github/hyperscan/BUILD/DIST/lib/pkgconfig"
	@echo 

clean:
	rm -f $(TARGET) test.o
