#sources
UPROC_SRC = src/htable.c src/uproc.c src/main.c src/utility.c
#object files
UPROC_OBJS = htable.o uproc.o utility.o
#test object files
UPROC_TEST_OBJS = uproc_test.o 
#executable
PROGRAM = libuproc.so
TEST_PROGRAMS = uproc_test
EXAMPLE_PROGRAMS = trivial
#compiler
CC = gcc

#includes
INCLUDE = -Iinclude
#linker params
LINKPARAMS = -fpic -lfuse -shared
#linker params for tests
LINKPARAMS_TEST = -L. -Wl,-rpath=. -fpic -lfuse -lpthread  -luproc 
LINKPARAMS_EXAMPLE = -L. -Wl,-rpath=. -fpic -lfuse  -luproc 
#options for development
CFLAGS = -g -Wall -Werror -fpic -D_FILE_OFFSET_BITS=64
TEST_CFLAGS =  -g -Wall -Werror -fpic -D_FILE_OFFSET_BITS=64 -D_UPROC_TEST
#options for release
#CFLAGS = --std=c++11 -g -O2 -Wall -Werror -fpic -shared

$(PROGRAM): $(UPROC_OBJS)
	$(CC) -o $(PROGRAM) $(UPROC_OBJS) $(CFLAGS) $(LINKPARAMS)

test: clean test_mode $(PROGRAM) $(UPROC_TEST_OBJS)
	$(CC) -o $(TEST_PROGRAMS) $(UPROC_OBJS) $(UPROC_TEST_OBJS) $(CFLAGS) $(LINKPARAMS_TEST)

example: $(PROGRAM) $(UPROC_OBJS) install $(EXAMPLE_PROGRAMS)
	@echo "done building examples: $(EXAMPLE_PROGRAMS)"

trivial: trivial.o
	$(CC) -o trivial  trivial.o $(UPROC_OBJS)  $(CFLAGS) $(LINKPARAMS_EXAMPLE)

trivial.o: example/trivial.c
	$(CC) -o trivial.o -c example/trivial.c $(CFLAGS) $(INCLUDE) $(LINKPARAMS_EXAMPLE)

test_mode:
	$(eval CFLAGS := $(TEST_CFLAGS))

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

install:
	mkdir -p /usr/local/include/uproc
	cp include/* /usr/local/include/uproc
	cp $(PROGRAM) /usr/local/lib

clean:
	rm -rf *.o
	rm -rf $(PROGRAM) $(TEST_PROGRAMS) $(EXAMPLE_PROGRAMS)

.PHONY: clean test test_mode example $(EXAMPLE_PROGRAMS) install
