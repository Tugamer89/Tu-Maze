#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "core/node.hh"
#include "render/gpumesh.hh"
#include "render/material.hh"
#include "render/mesh.hh"

struct Cell {
    bool wallTop = true;
    bool wallRight = true;
    bool wallBottom = true;
    bool wallLeft = true;
    bool visited = false;

    Cell() = default;
};

class Maze {
   public:
    static constexpr float CELL_SIZE = 1.0f;
    static constexpr float WALL_THICKNESS = 0.1f;
    static constexpr float WALL_HEIGHT = 1.0f;

   private:
    int width;
    int height;
    unsigned int currentSeed = 0;
    std::vector<Cell> grid;

    Cell& getCellModifiable(int x, int y);

    // Depth-First Search Maze Algorithm (Randomized backtracking)
    void generate(std::optional<unsigned int> seed);

   public:
    Maze(int width, int height, std::optional<unsigned int> seed = std::nullopt);

    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }
    [[nodiscard]] unsigned int getCurrentSeed() const { return currentSeed; }

    [[nodiscard]] const Cell& getCellAt(int x, int y) const;

    // Translates integer cell grid indices into 3D World space coordinates
    [[nodiscard]] glm::vec3 getCellWorldPosition(int x, int y) const;

    [[nodiscard]] glm::vec3 getStartWorldPosition() const {
        return getCellWorldPosition(width - 1, 0);
    }

    [[nodiscard]] glm::vec3 getGoalWorldPosition() const {
        return getCellWorldPosition(0, height - 1);
    }

    // Bakes the procedural map cells into a massive continuous GPU mesh chunk
    Node populateSceneNode(const Mesh& baseFloorMesh, const Mesh& baseWallMesh,
                           const Material& wallMat, const Material& floorMat,
                           std::unique_ptr<GPUMesh>& outBatchedWalls,
                           std::unique_ptr<GPUMesh>& outBatchedFloors) const;
};
