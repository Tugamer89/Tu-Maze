#ifndef MAZE_HH
#define MAZE_HH

#include <algorithm>
#include <array>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <stack>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "gpumesh.hh"
#include "material.hh"
#include "mesh.hh"
#include "node.hh"

struct Cell {
    bool wallTop = true;     // -Z direction
    bool wallRight = true;   // +X direction
    bool wallBottom = true;  // +Z direction
    bool wallLeft = true;    // -X direction
    bool visited = false;

    Cell() = default;
};

class Maze {
   public:
    static constexpr float CELL_SIZE = 1.0f;
    static constexpr float WALL_THICKNESS = 0.1f;
    static constexpr float WALL_HEIGHT = 1.0f;

    int width;
    int height;
    unsigned int currentSeed = 0;
    std::vector<Cell> grid;

    Maze(int width, int height, std::optional<unsigned int> seed = std::nullopt)
        : width(width), height(height) {
        grid.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        generate(seed);
    }

    // Returns the exact center world coordinate of a specific cell grid coordinate
    glm::vec3 getCellWorldPosition(int x, int y) const {
        float offsetX = (static_cast<float>(width) * CELL_SIZE) * 0.5f;
        float offsetZ = (static_cast<float>(height) * CELL_SIZE) * 0.5f;

        float cx = (static_cast<float>(x) * CELL_SIZE) - offsetX + (CELL_SIZE * 0.5f);
        float cz = (static_cast<float>(y) * CELL_SIZE) - offsetZ + (CELL_SIZE * 0.5f);

        return {cx, 0.5f, cz};  // Y=0.5 is exactly halfway up the walls (player height)
    }

    glm::vec3 getStartWorldPosition() const {
        // Top-Right: x = width - 1, y = 0
        return getCellWorldPosition(width - 1, 0);
    }

    glm::vec3 getGoalWorldPosition() const {
        // Bottom-Left: x = 0, y = height - 1
        return getCellWorldPosition(0, height - 1);
    }

    Node populateSceneNode(const Mesh& baseFloorMesh, const Mesh& baseWallMesh,
                           const Material& wallMat, const Material& floorMat,
                           std::unique_ptr<GPUMesh>& outBatchedWalls,
                           std::unique_ptr<GPUMesh>& outBatchedFloors) const {
        const float offsetX = (static_cast<float>(width) * CELL_SIZE) * 0.5f;
        const float offsetZ = (static_cast<float>(height) * CELL_SIZE) * 0.5f;

        Mesh combinedWalls;
        Mesh combinedFloors;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = grid[y * width + x];

                float cx = (static_cast<float>(x) * CELL_SIZE) - offsetX + (CELL_SIZE * 0.5f);
                float cz = (static_cast<float>(y) * CELL_SIZE) - offsetZ + (CELL_SIZE * 0.5f);
                glm::mat4 cellTransform = glm::translate(glm::mat4(1.0f), glm::vec3(cx, 0.0f, cz));

                // Add Floor
                combinedFloors.appendTransformed(baseFloorMesh, cellTransform);

                float wallLen = CELL_SIZE + WALL_THICKNESS;

                auto addWall = [&](float tx, float tz, float sx, float sz) {
                    glm::mat4 local = glm::translate(glm::mat4(1.0f), glm::vec3(tx, 0.0f, tz));
                    local = glm::scale(local, glm::vec3(sx, WALL_HEIGHT, sz));

                    glm::mat4 globalTransform = cellTransform * local;
                    combinedWalls.appendTransformed(baseWallMesh, globalTransform);
                };

                // Add Walls
                if (cell.wallTop && y == 0)
                    addWall(0.0f, -CELL_SIZE / 2.0f, wallLen, WALL_THICKNESS);
                if (cell.wallBottom) addWall(0.0f, CELL_SIZE / 2.0f, wallLen, WALL_THICKNESS);
                if (cell.wallLeft && x == 0)
                    addWall(-CELL_SIZE / 2.0f, 0.0f, WALL_THICKNESS, wallLen);
                if (cell.wallRight) addWall(CELL_SIZE / 2.0f, 0.0f, WALL_THICKNESS, wallLen);
            }
        }

        combinedWalls.compute_scale();
        combinedFloors.compute_scale();

        outBatchedWalls = std::make_unique<GPUMesh>(combinedWalls);
        outBatchedFloors = std::make_unique<GPUMesh>(combinedFloors);

        Node mazeRoot;

        // Global Walls Node
        Node wallsNode;
        wallsNode.mesh = outBatchedWalls.get();
        wallsNode.material = &wallMat;
        wallsNode.is_wall = true;

        // Global Floors Node
        Node floorsNode;
        floorsNode.mesh = outBatchedFloors.get();
        floorsNode.material = &floorMat;
        floorsNode.is_wall = false;

        mazeRoot.children.push_back(std::move(wallsNode));
        mazeRoot.children.push_back(std::move(floorsNode));

        return mazeRoot;
    }

   private:
    Cell& getCell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Maze coordinate out of bounds");
        return grid[y * width + x];
    }

    const Cell& getCell(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Maze coordinate out of bounds");
        return grid[y * width + x];
    }

    void generate(std::optional<unsigned int> seed) {
        std::stack<int> stack;

        currentSeed = seed.value_or(static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()));

        std::mt19937 rng(currentSeed);
        std::cout << "Maze generated with seed: " << currentSeed << std::endl;

        int currentIndex = 0;
        grid[currentIndex].visited = true;
        stack.push(currentIndex);

        struct DirInfo {
            int dx;
            int dy;
            bool Cell::*wallCurrent;
            bool Cell::*wallNext;
        };

        static const std::array<DirInfo, 4> dirTable = {{
            {0, -1, &Cell::wallTop, &Cell::wallBottom},  // TOP
            {1, 0, &Cell::wallRight, &Cell::wallLeft},   // RIGHT
            {0, 1, &Cell::wallBottom, &Cell::wallTop},   // BOTTOM
            {-1, 0, &Cell::wallLeft, &Cell::wallRight}   // LEFT
        }};

        std::vector<std::pair<int, int>> neighbors;  // <index, dirIndex>
        neighbors.reserve(4);

        while (!stack.empty()) {
            currentIndex = stack.top();
            int cx = currentIndex % width;
            int cy = currentIndex / width;

            neighbors.clear();

            for (int i = 0; i < 4; ++i) {
                int nx = cx + dirTable[i].dx;
                int ny = cy + dirTable[i].dy;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                    !grid[ny * width + nx].visited) {
                    neighbors.push_back({ny * width + nx, i});
                }
            }

            if (neighbors.empty()) {
                stack.pop();
                continue;
            }

            std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
            auto [nextIndex, dIdx] = neighbors[dist(rng)];

            // Remove walls
            grid[currentIndex].*dirTable[dIdx].wallCurrent = false;
            grid[nextIndex].*dirTable[dIdx].wallNext = false;

            grid[nextIndex].visited = true;
            stack.push(nextIndex);
        }
    }
};

#endif
