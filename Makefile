CC=gcc
CCOPTS=--std=gnu99 -Wall -Wextra -Wpedantic -Wc++-compat -g
AR=ar

CFLAGS = -Wall -g


HEADERS=fat.h
OBJS=fat.o
LIBS=fat.a
BINS=test_fat

.phony: clean all




all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

fat.a: $(OBJS) $(HEADERS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)


test_fat:  test_fat.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^



clean:
	rm -rf *.o *~ $(LIBS) $(BINS)