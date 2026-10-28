CC ?= gcc
CFLAGS := -shared -fPIC -nostdlib -nostdinc -I$(INCLUDE)
SONAME := libm.so
TARGET := $(OUT)/$(SONAME)
SRCS := $(wildcard $(SRC)/*.c)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -Wl,-soname,$(SONAME) -o $@ $^

clean:
	rm -f $(TARGET)