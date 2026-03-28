all: build/example

run: build/example
	./build/example

build/example: build/example.o
	gcc -o build/example build/example.o

build/example.o: example.c ccc_array.h
	mkdir -p build
	gcc -O0 -Wall -Wextra -ggdb -c example.c -o build/example.o

clean:
	rm -rf build

PHONY: all run clean