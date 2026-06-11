.PHONY: all configure build run clean

all: configure build run

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build --parallel --config Release

run:
	./build/bin/tu-maze

clean:
	rm -rf build