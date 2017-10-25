
CFLAGS += -DTRACE
CFLAGS += -DKK_VIRTIO
export CFLAGS

SRC += ./test

$(SRC):
	make -C $@

all: $(SRC)

.PHONY: $(SRC) all
all:
	@echo "I am all."
