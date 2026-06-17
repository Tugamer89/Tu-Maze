cmake_minimum_required(VERSION 3.5...4.0)
project(TU_Maze LANGUAGES CXX)

# Force C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warnings
if(NOT TARGET project_warnings)
    add_library(project_warnings INTERFACE)
    target_compile_options(project_warnings INTERFACE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall 
            -funsigned-char
            -Wno-error=narrowing
        >
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endif()

# Static linking for releases
if(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# ============================================================================
# SMART BINARY OUTPUT DIRECTORY CONFIGURATION
# ============================================================================
# If compiled as standalone development project, output directly to build/bin/
# If compiled inside the FCG_Stages root collector, output to build/bin/Stage_XX/
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    message(STATUS "[Build] Standalone development mode active. Binaries in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
else()
    get_filename_component(CURRENT_DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CURRENT_DIR_NAME})
    message(STATUS "[Build] Stage mode detected (${CURRENT_DIR_NAME}). Binaries in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

# ============================================================================
# THIRD-PARTY DEPENDENCIES (GLM & SFML 3.0) via FetchContent
# ============================================================================
include(FetchContent)

# 1. Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.3
    GIT_SHALLOW    ON
    SYSTEM
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glm)

# 2. Fetch SFML 3.0
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.2
    GIT_SHALLOW    ON
    SYSTEM
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(SFML)

# 3. Fetch Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.9
    GIT_SHALLOW    ON
    SYSTEM
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui)

# 4. Fetch ImGui-SFML
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(
    imgui_sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG        v3.0
    GIT_SHALLOW    ON
    SYSTEM
    EXCLUDE_FROM_ALL
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
FetchContent_MakeAvailable(imgui_sfml)

# ============================================================================
# DEVELOPMENT TARGET CONFIGURATION
# ============================================================================
# Collect all source files inside src/
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.cc" "src/*.hpp" "src/*.hh")

if(SRC_FILES)
    add_executable(tu-maze
        ${SRC_FILES}
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    
    # Link SFML 3.0 graphics modules and GLM
    target_link_libraries(tu-maze 
        PRIVATE 
            sfml-graphics
            sfml-window
            sfml-system
            glm::glm-header-only
            ImGui-SFML::ImGui-SFML
            project_warnings
    )
    
    # Include src/ directory and other necessary folders
    target_include_directories(tu-maze PRIVATE
        src
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
    )

    # Automatically copy resources/ directory to the binary directory post-build
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
        add_custom_command(
            TARGET tu-maze POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/resources"
                "$<TARGET_FILE_DIR:tu-maze>/resources"
            COMMENT "Copying assets and resources to build directory..."
        )
    endif()
else()
    message(WARNING "No source files detected in src/ folder. Please add main.cpp to start.")
endif()

# ============================================================================
# INSTALLATION & PACKAGING (CPack)
# ============================================================================
# Install the resources directory into the package . folder
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION .)
endif()

# Ensure the executable is also installed into the package . folder
if(TARGET tu-maze)
    install(TARGETS tu-maze DESTINATION .)
endif()

# Configure CPack variables
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "tu-maze")
set(CPACK_PACKAGE_VERSION "0.26.0") # x-release-please-version
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)
