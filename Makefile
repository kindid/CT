# do what thou wilt shall be the whole of the law

CC ?= cc
AR ?= ar
ARFLAGS ?= rcs
CFLAGS ?= -Wall -Wextra -Wshadow -std=c11

LIB = libctest.a
LIB_OBJ = ctest.o

.PHONY: all clean test

all: $(LIB)

$(LIB): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^

ctest.o: ctest.c ctest.h
	$(CC) $(CFLAGS) -c ctest.c

clean:
	rm -f $(LIB_OBJ) $(LIB)
	$(MAKE) -C test clean

test:
	$(MAKE) clean all
	$(MAKE) -C test test