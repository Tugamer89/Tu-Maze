# Explicitly define the default target (triggered when running just 'make')
.DEFAULT_GOAL := all

# Declaration of non-file (phony) targets to prevent conflicts with files of the same name
.PHONY: all format clean configure build run all-win configure-win build-win run-win

# ==========================================
# MAIN TARGETS
# ==========================================

# Native compilation and execution pipeline (Linux/WSL)
all: format configure build run

# Cross-compilation and execution pipeline (Windows)
all-win: format configure-win build-win run-win

# ==========================================
# SHARED TOOLS
# ==========================================

format:
	clang-format-15 -i src/*.cpp src/include/*.hh resources/shaders/*

# ==========================================
# NATIVE ENVIRONMENT (LINUX)
# ==========================================

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build --parallel --config Release

run:
	./build/bin/tu-maze

# ==========================================
# WINDOWS ENVIRONMENT (CROSS-COMPILATION)
# ==========================================

configure-win:
	cmake -B build-win --toolchain windows-toolchain.cmake -DCMAKE_BUILD_TYPE=Release

build-win:
	cmake --build build-win --parallel --config Release

run-win:
	./build-win/bin/tu-maze.exe

# ==========================================
# CLEANUP
# ==========================================

# Removes both native and cross-compiled build directories to ensure a clean state
clean:
	rm -rf build build-win
