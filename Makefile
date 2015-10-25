#sources
UPROC_SRC = src/htable.c src/uproc.c src/main.c src/utility.c
#object files
UPROC_OBJS = htable.o uproc.o main.o utility.o
#test object files
UPROC_TEST_OBJS = htable.o uproc.o uproc_test.o
#executable
PROGRAM = hello
TEST_PROGRAM = uproc_test
#compiler
CC = gcc

#includes
INCLUDE = -Iinclude
#linker params
LINKPARAMS = -fpic -lfuse
#linker params for tests
LINKPARAMS_TEST = -fpic -lfuse
#options for development
CFLAGS = -g -Wall -Werror -fpic -D_FILE_OFFSET_BITS=64
#options for release
#CFLAGS = --std=c++11 -g -O2 -Wall -Werror -fpic -shared

$(PROGRAM): $(UPROC_OBJS)
	$(CC) -o $(PROGRAM) $(UPROC_OBJS) $(CFLAGS) $(LINKPARAMS)

test: $(UPROC_TEST_OBJS)
	$(CC) -o $(TEST_PROGRAM) $(UPROC_TEST_OBJS) $(CFLAGS) $(LINKPARAMS)

htable.o: src/htable.c include/htable.h
	$(CC) -o htable.o -c src/htable.c $(CFLAGS) $(INCLUDE)

uproc.o: src/uproc.c include/uproc.h
	$(CC) -o uproc.o -c src/uproc.c $(CFLAGS) $(INCLUDE)

utility.o: src/utility.c
	$(CC) -o utility.o -c src/utility.c $(CFLAGS) $(INCLUDE)	
main.o: src/main.c
	$(CC) -o main.o -c src/main.c $(CFLAGS) $(INCLUDE)

uproc_test.o: tests/uproc_test.c
	$(CC) -o uproc_test.o -c tests/uproc_test.c $(CFLAGS) $(INCLUDE)

clean:
	rm -rf *.o
	rm -rf $(PROGRAM) $(TEST_PROGRAM)

.PHONY: clean test
