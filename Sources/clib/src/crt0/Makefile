CC      ?= gcc
AS      ?= as
ASFLAGS  =

ifeq ($(shell $(CC) -dumpmachine | grep -c x86_64), 1)
    CRT_SRC := $(SRC)/crt0_x86_64.s
else ifeq ($(shell $(CC) -dumpmachine | grep -c aarch64), 1)
    CRT_SRC := $(SRC)/crt0_aarch64.s
endif

CRT_OBJ := $(OUT)/crt0.o

.PHONY: all clean

all: $(CRT_OBJ)

$(CRT_OBJ): $(CRT_SRC)
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -f $(CRT_OBJ)