# Explicitly define the default target (triggered when running just 'make')
.DEFAULT_GOAL := all

# Declaration of non-file (phony) targets to prevent conflicts with files of the same name
.PHONY: all format clean configure build run package all-win configure-win build-win run-win package-win all-mac configure-mac build-mac package-mac

# ==========================================
# MAIN TARGETS
# ==========================================

# Native compilation and execution pipeline
all: format configure build run

# Cross-compilation pipeline (Windows)
all-win: format configure-win build-win run-win

# Cross-compilation pipeline (macOS via osxcross)
all-mac: format configure-mac build-mac

# ==========================================
# SHARED TOOLS
# ==========================================

format:
	clang-format-15 -i src/*.cpp src/*/*.hh resources/shaders/*

# ==========================================
# NATIVE ENVIRONMENT
# ==========================================

configure:
	cmake -B build -DCMAKE_BUILD_TYPE=Release

build:
	cmake --build build --parallel --config Release

run:
	./build/bin/tu-maze

package: build
	cd build && cpack -G ZIP -C Release

# ==========================================
# WINDOWS ENVIRONMENT (CROSS-COMPILATION)
# ==========================================

configure-win:
	cmake -B build-win --toolchain windows-toolchain.cmake -DCMAKE_BUILD_TYPE=Release

build-win:
	cmake --build build-win --parallel --config Release

run-win:
	./build-win/bin/tu-maze.exe

package-win: build-win
	cd build-win && cpack -G ZIP -C Release

# ==========================================
# MACOS ENVIRONMENT (CROSS-COMPILATION)
# ==========================================

configure-mac:
	cmake -B build-mac --toolchain macos-toolchain.cmake -DCMAKE_BUILD_TYPE=Release

build-mac:
	cmake --build build-mac --parallel --config Release

package-mac: build-mac
	cd build-mac && cpack -G ZIP -C Release

# ==========================================
# CLEANUP
# ==========================================

# Removes native and cross-compiled build directories to ensure a clean state
clean:
	rm -rf build build-win build-mac
