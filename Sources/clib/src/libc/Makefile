CC      ?= gcc
CFLAGS   = -shared -fPIC -nostdlib -nostdinc -isystem $(shell $(CC) -print-file-name='include') -I$(INCLUDE)
SONAME  := libc.so
TARGET  := $(OUT)/$(SONAME)
SRCS    := $(shell find $(SRC) -iname "*.c")

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -Wl,-soname,$(SONAME) -o $@ $^

clean:
	rm -f $(TARGET)