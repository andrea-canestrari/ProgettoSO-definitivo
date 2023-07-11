CC= gcc
AR=ar
CCOPTS=--std=c99 -Wall -Wstrict-prototypes -Wextra -Wpedantic -Wc++-compat -D_POSIX_C_SOURCE=200112L -g

HEADERS=fat.h
OBJS=fat.o
LIBS=fat.a
BINS=test_fat

.phony: clean all

all: $(BINS)

%.o: %.c $(HEADERS)
    $(CC) $(CCOPTS) -c -o $@ $<

test_fat: test_fat.c $(LIBS)
    $(CC) $(CCOPTS) -o $@ $^

clean:
    rm -rf *.o *~ $(LIBS) $(BINS)