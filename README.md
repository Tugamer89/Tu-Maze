# Tu-Maze

**Tu-Maze** is an enhanced, high-performance evolution of the [Maze project](https://github.com/Tugamer89/Maze), completely rewritten and accelerated using modern graphics libraries: **SFML 3.0**, **GLM** and **OpenGL 4.1**.

The core rendering engine features strict C++20 standard compliance, real-time animation clock tracking, and an automated multi-stage compilation framework.

---

## Installation & Build Instructions

### Prerequisites

Ensure you have CMake (v3.5 or higher) and a C++20 compliant compiler (such as GCC 15+, Clang, or MSVC) installed on your system.

### Standalone Development Mode

To compile and run the main branch of the project in standalone development mode, use the following commands from the root directory:

#### Configure the Project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
# or: make configure
```

#### Compile the Project

```bash
cmake --build build --parallel --config Release
# or: make build
```

Upon successful compilation, the compiled standalone binary along with its assets will be automatically placed in the centralized binary output folder: `build/bin/`.

#### Run the Application

```bash
./build/bin/tu-maze
# or: make run
```

Alternatively, you can chain all the previuos steps in one single command:

```bash
make configure build run
```

## Managing and Compiling Project Stages (Export Workflow)

This repository tracks the evolutionary milestones of the project using semantic versioning (SemVer) Git tags. A centralized automation utility script, `export.sh`, handles checking out historic milestones, filtering configuration configurations, and compiling them into a single parallelized collection.

### 1. Exporting Milestones (Stages)

To extract all tagged minor versions of the project into individual modular subdirectories (`FCG_Stages/Stage_01`, `FCG_Stages/Stage_02`, etc.) and automatically generate a unified wrapper `CMakeLists.txt`, execute:

```bash
./export.sh x
# or: ./export.sh export
```

This script parses your Git tag history, safely copies tracked source files for each valid release version, and returns your workspace seamlessly to your active development branch.

### 2. Centralized Multi-Stage Compilation

Once exported, you can compile all evolutionary stages simultaneously in parallel through a single unified build tree by running:

```bash
./export.sh c
# or: ./export.sh compile
```

Alternatively, you can chain both the export and the compilation pipelines sequentially in one single command:

```bash
./export.sh a
# or: ./export.sh all
```

### 3. Running an Independent Stage Executable

Once compiled via the centralized multi-stage workspace, the independent executables for each specific milestone are isolated inside the build directory:

```bash
# To run Stage 1
cd FCG_Stages/build/bin/Stage_01
./tu-maze_01

# To run Stage 2
cd ../Stage_02
./tu-maze_02

# And so on...
```

Or directly from the source directories:

```bash
# To run Stage 1
cd FCG_Stages/Stage_01
../build/bin/tu-maze_01
# or from 

# To run Stage 2
cd ../Stage_02
../build/bin/tu-maze_02

# And so on...
```

## User Interface & Controls

As the project evolves, the control scheme expands. Below are the specific commands mapped to each stage.

### Stage 25 (v0.26.x) - *Latest*

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **Actions:** <kbd>F</kbd> (Toggle torch)
* **System:** <kbd>Esc</kbd> (Pause Menu, close Leaderboard/Settings, Exit Main Menu)
* **Mouse:** **Drag** (Look around / Move camera)

### Stage 22 (v0.23.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **System:** <kbd>Esc</kbd> (Pause Menu, close Leaderboard/Settings, Exit Main Menu)
* **Mouse:** **Drag** (Look around / Move camera)

### Stage 21 (v0.22.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **System:** <kbd>Esc</kbd> (Pause Menu, close Leaderboard, Exit Main Menu)
* **Mouse:** **Drag** (Look around / Move camera)

### Stage 19 (v0.20.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **System:** <kbd>Esc</kbd> (Toggle Pause Menu)
* **Mouse:** **Drag** (Look around / Move camera)

### Stage 18 (v0.19.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **System:** <kbd>Esc</kbd> (Close application)
* **Mouse:** <kbd>Left-Click</kbd> + **Drag** (Look around / Move camera)

### Stage 15 (v0.16.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move) | <kbd>Shift</kbd> (Sprint)
* **Shaders:** <kbd>P</kbd> (Phong) | <kbd>F</kbd> (Flat)
* **System:** <kbd>Esc</kbd> (Close application)
* **Mouse:** <kbd>Left-Click</kbd> + **Drag** (Look around / Move camera)

### Stage 7 (v0.8.x)

* **Movement:** <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> (Move)
* **Shaders:** <kbd>P</kbd> (Phong) | <kbd>F</kbd> (Flat)
* **System:** <kbd>Esc</kbd> (Close application)
* **Mouse:** <kbd>Left-Click</kbd> + **Drag** (Look around / Move camera)

### Stage 4 (v0.5.x)

* **Shaders:** <kbd>P</kbd> (Phong) | <kbd>F</kbd> (Flat)
* **System:** <kbd>Esc</kbd> (Close application)
* **Mouse:** <kbd>Left-Click</kbd> + **Drag** (Rotate object) | <kbd>Alt</kbd> + <kbd>Left-Click</kbd> + **Drag** (Move object)

### Stage 1 (v0.2.x)

* **Shaders:** <kbd>G</kbd> (Gouraud) | <kbd>P</kbd> (Phong) | <kbd>F</kbd> (Flat) | <kbd>C</kbd> (Normal)
* **System:** <kbd>Esc</kbd> (Close application)
* **Mouse:** <kbd>Left-Click</kbd> + **Drag** (Rotate object) | <kbd>Alt</kbd> + <kbd>Left-Click</kbd> + **Drag** (Move object)
