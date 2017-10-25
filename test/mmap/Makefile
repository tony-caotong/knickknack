
TARGET=test

OBJECT := $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(TARGET)
