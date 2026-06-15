#ifndef SCENE_HH
#define SCENE_HH

#include <glm/mat4x4.hpp>
#include <string>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "camera.hh"
#include "gpumesh.hh"
#include "lights.hh"
#include "matrices.hh"
#include "maze.hh"
#include "node.hh"

class Scene {
   public:
    Camera camera;
    Lights lights;

    // Core Scene Graph
    Node root;
    Node goalNode;  // Special node for gameplay

   private:
    GLint model_loc;
    GLint vp_loc;
    GLint tr_inv_model_loc;
    MaterialLocations mat_locs;

   public:
    explicit Scene(const Shaders& shaders) : camera(shaders), lights(shaders) {
        locations(shaders);
        update_all();
    }

    void locations(const Shaders& shaders) {
        camera.locations(shaders);
        lights.locations(shaders);
        model_loc = glGetUniformLocation(shaders.program, "model");
        vp_loc = glGetUniformLocation(shaders.program, "vp");
        tr_inv_model_loc = glGetUniformLocation(shaders.program, "tr_inv_model");

        // Cache the material uniforms here so we only query them once
        mat_locs.diffuse_loc = glGetUniformLocation(shaders.program, "material.diffuse");
        mat_locs.ambient_loc = glGetUniformLocation(shaders.program, "material.ambient");
        mat_locs.specular_loc = glGetUniformLocation(shaders.program, "material.specular");
        mat_locs.shininess_loc = glGetUniformLocation(shaders.program, "material.shininess");
        mat_locs.alpha_loc = glGetUniformLocation(shaders.program, "material.alpha");

        mat_locs.use_textures_loc = glGetUniformLocation(shaders.program, "useTextures");

        glUniform1i(glGetUniformLocation(shaders.program, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaders.program, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaders.program, "roughnessMap"), 2);
    }

    void update_all() {
        camera.projection();
        lights.parameters();
        lights.position(camera.inv_v);
    }

    void build_static_tree() {
        root.updateTransforms();
        goalNode.updateTransforms();
    }

    // Handles logic that changes over time (animations)
    void update_gameplay(sf::Time dt, const glm::vec3& goalWorldPos) {
        static float bobAngle = 0.0f;
        static float rotAngle = 0.0f;
        const float TWO_PI = 2.0f * glm::pi<float>();

        bobAngle += dt.asSeconds() * 3.0f;
        rotAngle += dt.asSeconds() * 2.0f;

        if (bobAngle > TWO_PI) bobAngle -= TWO_PI;
        if (rotAngle > TWO_PI) rotAngle -= TWO_PI;

        // Animate the goal marker
        glm::mat4 goalTransform = glm::translate(
            glm::mat4(1.0f), goalWorldPos + glm::vec3(0.0f, std::sin(bobAngle) * 0.1f, 0.0f));
        goalTransform = glm::rotate(goalTransform, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        goalTransform = glm::scale(goalTransform, glm::vec3(0.4f));

        goalNode.localMatrix = goalTransform;
        goalNode.updateTransforms();  // Apply rotation/position safely
    }

    // Checks game rules against scene state
    bool check_win_condition(const glm::vec3& goalPos) const {
        float distToGoal = glm::distance(camera.getPosition(), goalPos);
        return distToGoal <= Maze::CELL_SIZE * 0.5f;
    }

    void draw() {
        glUniformMatrix4fv(vp_loc, 1, GL_FALSE, glm::value_ptr(camera.vp));

        root.draw(model_loc, tr_inv_model_loc, camera.frustumPlanes, mat_locs);
        goalNode.draw(model_loc, tr_inv_model_loc, camera.frustumPlanes, mat_locs);
    }
};

#endif
