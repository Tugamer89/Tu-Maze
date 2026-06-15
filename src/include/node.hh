#ifndef NODE_HH
#define NODE_HH

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

class Node {
   public:
    glm::mat4 localMatrix = glm::mat4(glm::mat4(1.0f));
    glm::mat4 globalMatrix = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(1.0f);
    GPUMesh* mesh = nullptr;
    Material material;
    std::vector<Node> children;

    bool is_wall = false;
    bool is_goal = false;

    Node() = default;

    void updateTransforms(const glm::mat4& parentMatrix = glm::mat4(1.0f)) {
        globalMatrix = parentMatrix * localMatrix;
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(globalMatrix)));

        for (Node& child : children) {
            child.updateTransforms(globalMatrix);
        }
    }

    // High performance fast-path drawing with culling
    void drawMinimap(GLint model_loc, GLint color_loc, const glm::vec3& camPos,
                     float cullRadius) const {
        if (mesh) {
            glm::vec3 worldCenter(globalMatrix * glm::vec4(mesh->center, 1.0f));

            // Squared distance on XZ
            float dx = worldCenter.x - camPos.x;
            float dz = worldCenter.z - camPos.z;
            float distSq = (dx * dx) + (dz * dz);

            // Ray estimation
            float nodeRadius = mesh->extent * 2.0f;
            float maxRenderDist = cullRadius + nodeRadius;

            if (distSq < (maxRenderDist * maxRenderDist)) {
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(globalMatrix));

                if (is_goal)
                    glUniform3f(color_loc, 0.2f, 0.9f, 0.3f);  // Bright neon green goal
                else if (is_wall)
                    glUniform3f(color_loc, 0.85f, 0.85f, 0.85f);  // Light grey
                else
                    glUniform3f(color_loc, 0.15f, 0.15f, 0.15f);  // Dark grey

                mesh->draw();
            }
        }

        for (const Node& child : children) {
            child.drawMinimap(model_loc, color_loc, camPos, cullRadius);
        }
    }

    // Recursively draw this node and all its children
    void draw(GLint model_loc, GLint tr_inv_model_loc,
              const std::array<glm::vec4, 6>& frustumPlanes,
              const MaterialLocations& mat_locs,
              const Material** activeMaterial) const {
        if (mesh) {
            // Transform center point to world space
            glm::vec3 worldCenter(globalMatrix * glm::vec4(mesh->center, 1.0f));

            // Magnitude squared of the transformed basis vectors gives us the squared scaling
            // factors
            float scaleX2 = glm::dot(glm::vec3(globalMatrix[0]), glm::vec3(globalMatrix[0]));
            float scaleY2 = glm::dot(glm::vec3(globalMatrix[1]), glm::vec3(globalMatrix[1]));
            float scaleZ2 = glm::dot(glm::vec3(globalMatrix[2]), glm::vec3(globalMatrix[2]));

            // Approximate maximum local scaling to apply to the radius of the bounding sphere
            float maxScale = std::sqrt(std::max({scaleX2, scaleY2, scaleZ2}));

            float worldRadius = mesh->extent * maxScale;

            bool insideFrustum = true;
            for (const auto& plane : frustumPlanes) {
                // Check distance against the normal of the plane
                if (glm::dot(glm::vec3(plane), worldCenter) + plane.w < -worldRadius) {
                    insideFrustum = false;  // Outside viewing area!
                    break;
                }
            }

            // Only draw if inside the field of view
            if (insideFrustum) {
                glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(globalMatrix));
                glUniformMatrix3fv(tr_inv_model_loc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                if (*activeMaterial == nullptr || **activeMaterial != material) {
                    if (*activeMaterial != nullptr) {
                        (*activeMaterial)->unbind();
                    }
                    material.bind(mat_locs);
                    *activeMaterial = &material;
                }
                mesh->draw();
            }
        }

        // Recursively draw all children passing our computed global matrix as their parent matrix
        for (const Node& child : children) {
            child.draw(model_loc, tr_inv_model_loc, frustumPlanes, mat_locs, activeMaterial);
        }
    }
};

#endif
