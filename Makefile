.PHONY: all configure build run clean

all: format configure build run

format:
	clang-format-15 -i src/*.cpp src/include/*.hh resources/shaders/*

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build --parallel --config Release

run:
	./build/bin/tu-maze

clean:
	rm -rf build