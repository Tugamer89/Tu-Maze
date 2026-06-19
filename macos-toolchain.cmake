# OS target
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.3" CACHE STRING "Minimum macOS version" FORCE)

# Enable Universal Binary generation (Apple Silicon ARM64 + Intel x86_64)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for macOS" FORCE)

# Cross-compilers Apple Darwin
set(CMAKE_C_COMPILER x86_64-apple-darwin25.1-clang)
set(CMAKE_CXX_COMPILER x86_64-apple-darwin25.1-clang++)
set(CMAKE_AR x86_64-apple-darwin25.1-ar)
set(CMAKE_RANLIB x86_64-apple-darwin25.1-ranlib)

# Search rules for programs, libraries and headers
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)