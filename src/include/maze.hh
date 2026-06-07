#ifndef MAZE_HH
#define MAZE_HH

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdexcept>
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

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Cell& cell = getCell(x, y);

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
                if (cell.wallTop) {
                    Node wallNode;
                    wallNode.mesh = wallMesh;
                    glm::mat4 local =
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -cellSize / 2.0f));
                    wallNode.localMatrix = glm::scale(
                        local, glm::vec3(cellSize + wallThickness, wallHeight, wallThickness));
                    cellNode.children.push_back(std::move(wallNode));
                }

                if (cell.wallBottom) {
                    Node wallNode;
                    wallNode.mesh = wallMesh;
                    glm::mat4 local =
                        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, cellSize / 2.0f));
                    wallNode.localMatrix = glm::scale(
                        local, glm::vec3(cellSize + wallThickness, wallHeight, wallThickness));
                    cellNode.children.push_back(std::move(wallNode));
                }

                if (cell.wallLeft) {
                    Node wallNode;
                    wallNode.mesh = wallMesh;
                    glm::mat4 local =
                        glm::translate(glm::mat4(1.0f), glm::vec3(-cellSize / 2.0f, 0.0f, 0.0f));
                    wallNode.localMatrix = glm::scale(
                        local, glm::vec3(wallThickness, wallHeight, cellSize + wallThickness));
                    cellNode.children.push_back(std::move(wallNode));
                }

                if (cell.wallRight) {
                    Node wallNode;
                    wallNode.mesh = wallMesh;
                    glm::mat4 local =
                        glm::translate(glm::mat4(1.0f), glm::vec3(cellSize / 2.0f, 0.0f, 0.0f));
                    wallNode.localMatrix = glm::scale(
                        local, glm::vec3(wallThickness, wallHeight, cellSize + wallThickness));
                    cellNode.children.push_back(std::move(wallNode));
                }

                mazeRoot.children.push_back(std::move(cellNode));
            }
        }
        return mazeRoot;
    }

   private:
    void generate() {
        for (auto& cell : grid) {
            cell.wallTop = true;
            cell.wallRight = true;
            cell.wallBottom = true;
            cell.wallLeft = true;
            cell.visited = false;
        }
    }

    Cell& getCell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Coordinate del labirinto fuori dai limiti consentiti.");
        }

        return grid[y * width + x];
    }

    const Cell& getCell(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Coordinate del labirinto fuori dai limiti consentiti.");
        }

        return grid[y * width + x];
    }
};

#endif
