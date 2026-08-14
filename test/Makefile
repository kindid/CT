# do what thou wilt shall be the whole of the law

CC ?= cc
CFLAGS ?= -Wall -Wextra -Wshadow -std=c11
CPPFLAGS ?= -I..

TEST_BIN = ctest_test
TEST_OBJ = ctest_test.o
TEST_SCRIPT = test_ctest.sh

.PHONY: all clean test

all: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) ../libctest.a
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) ../libctest.a

ctest_test.o: ctest_test.c ../ctest.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c ctest_test.c

clean:
	rm -f $(TEST_OBJ) $(TEST_BIN)

test: all
	./$(TEST_SCRIPT)
