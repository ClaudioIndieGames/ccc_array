all: build/example_c build/example_cpp build/example_cuda

run_c: build/example_c
	./build/example_c

run_cpp: build/example_cpp
	./build/example_cpp

run_cuda: build/example_cuda
	./build/example_cuda

build/example_c: build/example_c.o
	gcc -o build/example_c build/example_c.o

build/example_cpp: build/example_cpp.o
	g++ -o build/example_cpp build/example_cpp.o

build/example_cuda: build/example_cuda.o
	nvcc -o build/example_cuda build/example_cuda.o

build/example_c.o: example.c ccc_array.h
	mkdir -p build
	gcc  -O0 -ggdb -Wall -Wextra -Wno-override-init -c example.c -o build/example_c.o

build/example_cpp.o: example.c ccc_array.h
	mkdir -p build
	g++  -O0 -ggdb -Wall -Wextra -Wno-missing-field-initializers -c example.c -o build/example_cpp.o

build/example_cuda.o: example.c ccc_array.h
	mkdir -p build
	nvcc -x cu -Xcompiler "-O0 -ggdb -Wall -Wextra -Wno-missing-field-initializers" -c example.c -o build/example_cuda.o

clean:
	rm -rf build

PHONY: all run_c run_cpp run_cuda clean