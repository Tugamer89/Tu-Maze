#include "core/node.hpp"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

void Node::updateTransforms(const glm::mat4& parentMatrix /*= glm::mat4(1.0f)*/) {
    globalMatrix = parentMatrix * localMatrix;

    if (mesh) {
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(globalMatrix)));
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

void Node::drawMinimap(GLint model_loc, GLint color_loc, const glm::vec3& camPos,
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

void Node::draw(GLint model_loc, GLint tr_inv_model_loc,
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
