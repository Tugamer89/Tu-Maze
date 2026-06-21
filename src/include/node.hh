#pragma once

#include <algorithm>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "gpumesh.hh"
#include "material.hh"

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
    void updateTransforms(const glm::mat4& parentMatrix = glm::mat4(1.0f)) {
        globalMatrix = parentMatrix * localMatrix;
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(globalMatrix)));

        if (mesh) {
            worldCenter = glm::vec3(globalMatrix * glm::vec4(mesh->getCenter(), 1.0f));

            float scaleX2 = glm::dot(glm::vec3(globalMatrix[0]), glm::vec3(globalMatrix[0]));
            float scaleY2 = glm::dot(glm::vec3(globalMatrix[1]), glm::vec3(globalMatrix[1]));
            float scaleZ2 = glm::dot(glm::vec3(globalMatrix[2]), glm::vec3(globalMatrix[2]));

            float maxScale = std::sqrt(std::max({scaleX2, scaleY2, scaleZ2}));
            worldRadius = mesh->getExtent() * maxScale;
        }

        for (Node& child : children) {
            child.updateTransforms(globalMatrix);
        }
    }

    void drawMinimap(GLint model_loc, GLint color_loc, const glm::vec3& camPos,
                     float cullRadius) const {
        if (mesh) {
            float dx = worldCenter.x - camPos.x;
            float dz = worldCenter.z - camPos.z;
            float distSq = (dx * dx) + (dz * dz);

            float maxRenderDist = cullRadius + worldRadius;

            // Simple radial culling
            if (distSq < (maxRenderDist * maxRenderDist)) {
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(globalMatrix));

                if (is_goal)
                    glUniform3f(color_loc, 0.2f, 0.9f, 0.3f);
                else if (is_wall)
                    glUniform3f(color_loc, 0.85f, 0.85f, 0.85f);
                else
                    glUniform3f(color_loc, 0.15f, 0.15f, 0.15f);

                mesh->draw();
            }
        }

        for (const Node& child : children) {
            child.drawMinimap(model_loc, color_loc, camPos, cullRadius);
        }
    }

    // Main render pass utilizing Sphere-to-Plane Frustum Culling
    void draw(GLint model_loc, GLint tr_inv_model_loc,
              const std::array<glm::vec4, 6>& frustumPlanes, const MaterialLocations& mat_locs,
              const Material*& currentMat) const {
        if (mesh) {
            bool insideFrustum = true;
            for (const auto& plane : frustumPlanes) {
                // If the sphere's distance to the plane is deeper than its negative radius, it is
                // completely behind the plane.
                if (glm::dot(glm::vec3(plane), worldCenter) + plane.w < -worldRadius) {
                    insideFrustum = false;
                    break;
                }
            }

            if (insideFrustum) {
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(globalMatrix));
                glUniformMatrix3fv(tr_inv_model_loc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                // State filtering: only bind material if it differs from the active one
                if (material && material != currentMat) {
                    material->bind(mat_locs);
                    currentMat = material;
                }

                mesh->draw();
            }
        }

        for (const Node& child : children) {
            child.draw(model_loc, tr_inv_model_loc, frustumPlanes, mat_locs, currentMat);
        }
    }
};
