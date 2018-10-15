
CC = gcc

all: test
	./test

test: AllTests.c CuTest.c CuTestTest.c
	$(CC) $^ -o $@

clean:
	rm -f test*
