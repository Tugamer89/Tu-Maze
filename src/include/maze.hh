#ifndef MAZE_HH
#define MAZE_HH

#include <algorithm>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <stack>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "gpumesh.hh"
#include "matrices.hh"
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
    int width;
    int height;
    std::vector<Cell> grid;

    Maze(int width, int height) : width(width), height(height) {
        grid.resize(width * height);
        generate();
    }

    Node populateSceneNode(GPUMesh* floorMesh, GPUMesh* wallMesh) const {
        const float cellSize = 1.0f;
        const float wallThickness = 0.1f;
        const float wallHeight = 1.0f;

        // Calculate offsets to center the maze at the origin
        const float offsetX = (static_cast<float>(width) * cellSize) * 0.5f;
        const float offsetZ = (static_cast<float>(height) * cellSize) * 0.5f;

        Node mazeRoot;

        auto addWall = [&wallMesh, &wallHeight](Node& parent, float tx, float tz, float sx,
                                                float sz) {
            Node wallNode;
            wallNode.mesh = wallMesh;
            glm::mat4 local = glm::translate(glm::mat4(1.0f), glm::vec3(tx, 0.0f, tz));
            wallNode.localMatrix = glm::scale(local, glm::vec3(sx, wallHeight, sz));
            parent.children.push_back(std::move(wallNode));
        };

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = grid[y * width + x];

                // Create a parent node for this specific cell
                Node cellNode;
                // Center the cell relative to the origin
                cellNode.localMatrix = glm::translate(
                    glm::mat4(1.0f),
                    glm::vec3((static_cast<float>(x) * cellSize) - offsetX + (cellSize * 0.5f),
                              0.0f,
                              (static_cast<float>(y) * cellSize) - offsetZ + (cellSize * 0.5f)));

                // Add Floor (Relative to the cell, so Identity matrix)
                Node floorNode;
                floorNode.mesh = floorMesh;
                cellNode.children.push_back(std::move(floorNode));

                // Add Walls (Relative to the cell)
                if (cell.wallTop)
                    addWall(cellNode, 0.0f, -cellSize / 2.0f, cellSize + wallThickness,
                            wallThickness);
                if (cell.wallBottom)
                    addWall(cellNode, 0.0f, cellSize / 2.0f, cellSize + wallThickness,
                            wallThickness);
                if (cell.wallLeft)
                    addWall(cellNode, -cellSize / 2.0f, 0.0f, wallThickness,
                            cellSize + wallThickness);
                if (cell.wallRight)
                    addWall(cellNode, cellSize / 2.0f, 0.0f, wallThickness,
                            cellSize + wallThickness);

                mazeRoot.children.push_back(std::move(cellNode));
            }
        }
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

    void generate() {
        std::stack<int> stack;

        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937 rng(static_cast<unsigned int>(seed));
        std::cout << "Maze generated with seed: " << seed << std::endl;

        int currentIndex = 0;
        grid[currentIndex].visited = true;
        stack.push(currentIndex);

        enum class Direction { TOP, RIGHT, BOTTOM, LEFT };

        // List of neighbors: <index, direction>
        std::vector<std::pair<int, Direction>> neighbors;
        neighbors.reserve(4);

        using enum Direction;

        while (!stack.empty()) {
            currentIndex = stack.top();
            int cx = currentIndex % width;
            int cy = currentIndex / width;

            neighbors.clear();

            if (cy > 0 && !getCell(cx, cy - 1).visited)
                neighbors.push_back({(cy - 1) * width + cx, TOP});
            if (cx < width - 1 && !getCell(cx + 1, cy).visited)
                neighbors.push_back({cy * width + (cx + 1), RIGHT});
            if (cy < height - 1 && !getCell(cx, cy + 1).visited)
                neighbors.push_back({(cy + 1) * width + cx, BOTTOM});
            if (cx > 0 && !getCell(cx - 1, cy).visited)
                neighbors.push_back({cy * width + (cx - 1), LEFT});

            if (neighbors.empty()) {
                stack.pop();
                continue;
            }

            std::uniform_int_distribution<size_t> dist(0, neighbors.size() - 1);
            auto [nextIndex, dir] = neighbors[dist(rng)];

            // Remove walls
            Cell& curr = grid[currentIndex];
            Cell& next = grid[nextIndex];

            switch (dir) {
                case TOP:
                    curr.wallTop = false;
                    next.wallBottom = false;
                    break;
                case RIGHT:
                    curr.wallRight = false;
                    next.wallLeft = false;
                    break;
                case BOTTOM:
                    curr.wallBottom = false;
                    next.wallTop = false;
                    break;
                case LEFT:
                    curr.wallLeft = false;
                    next.wallRight = false;
                    break;
                default:
                    break;
            }

            next.visited = true;
            stack.push(nextIndex);
        }
    }
};

#endif
