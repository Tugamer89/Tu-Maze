#pragma once

#include <glm/mat4x4.hpp>
#include <vector>

#include "glad/gl.h"
#include "render/gpumesh.hpp"
#include "render/material.hpp"

// Scene Graph node allowing hierarchical rendering and transformations
class Node {
   private:
    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 globalMatrix = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(1.0f);

    glm::vec3 worldCenter = glm::vec3(0.0f);
    float worldRadius = 0.0f;

    GPUMesh* mesh = nullptr;
    const Material* material = nullptr;
    std::vector<Node> children;

    bool is_wall = false;
    bool is_goal = false;

   public:
    Node() = default;

    void setLocalMatrix(const glm::mat4& mat) { localMatrix = mat; }
    [[nodiscard]] const glm::mat4& getLocalMatrix() const { return localMatrix; }

    void setMesh(GPUMesh* mesh) { this->mesh = mesh; }
    void setMaterial(const Material* mat) { material = mat; }
    void addChild(Node&& child) { children.push_back(std::move(child)); }
    void clearChildren() { children.clear(); }

    void setIsWall(bool val) { is_wall = val; }
    void setIsGoal(bool val) { is_goal = val; }

    // Recursively bakes local transforms down the tree and updates bounding volumes
    void updateTransforms(const glm::mat4& parentMatrix = glm::mat4(1.0f));

    void drawMinimap(GLint model_loc, GLint color_loc, const glm::vec3& camPos,
                     float cullRadius) const;

    // Main render pass utilizing Sphere-to-Plane Frustum Culling
    void draw(GLint model_loc, GLint tr_inv_model_loc,
              const std::array<glm::vec4, 6>& frustumPlanes, const MaterialLocations& mat_locs,
              const Material*& currentMat) const;
};
