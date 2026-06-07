#ifndef NODE_HH
#define NODE_HH

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "gpumesh.hh"

class Node {
   public:
    glm::mat4 localMatrix = glm::mat4(glm::mat4(1.0f));
    GPUMesh* mesh = nullptr;
    std::vector<Node> children;

    Node() = default;

    // Recursively draw this node and all its children
    void draw(GLint model_loc, GLint tr_inv_model_loc, const glm::mat4& parentMatrix) const {
        glm::mat4 globalMatrix = parentMatrix * localMatrix;

        if (mesh) {
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(globalMatrix));

            // Calculate and send the normal matrix (transpose of the inverse of the top-left 3x3
            // model matrix)
            glm::mat3 tr_inv_model = glm::transpose(glm::inverse(glm::mat3(globalMatrix)));
            glUniformMatrix3fv(tr_inv_model_loc, 1, GL_FALSE, glm::value_ptr(tr_inv_model));

            mesh->draw();
        }

        // Recursively draw all children passing our computed global matrix as their parent matrix
        for (const Node& child : children) {
            child.draw(model_loc, tr_inv_model_loc, globalMatrix);
        }
    }
};

#endif
