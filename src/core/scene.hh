#pragma once

#include "core/camera.hh"
#include "core/node.hh"
#include "glad/gl.h"
#include "render/lights.hh"

// Manages the hierarchical node tree, rendering passes, and global uniforms
class Scene {
   private:
    Camera camera;
    Lights lights;

    Node root;
    Node goalNode;

    GLint model_loc = -1;
    GLint vp_loc = -1;
    GLint tr_inv_model_loc = -1;
    MaterialLocations mat_locs;

   public:
    explicit Scene(const Shaders& shaders);

    Camera& getCamera() { return camera; }
    [[nodiscard]] const Camera& getCamera() const { return camera; }

    Lights& getLights() { return lights; }
    [[nodiscard]] const Lights& getLights() const { return lights; }

    Node& getRoot() { return root; }
    [[nodiscard]] const Node& getRoot() const { return root; }

    Node& getGoalNode() { return goalNode; }
    [[nodiscard]] const Node& getGoalNode() const { return goalNode; }

    // Queries and caches all necessary uniform locations from the active shader program
    void locations(const Shaders& shaders);

    void update_all();

    void build_static_tree();

    // Processes animations and state that change strictly over time
    void update_gameplay(sf::Time dt, const glm::vec3& goalWorldPos);

    [[nodiscard]] bool check_win_condition(const glm::vec3& goalPos) const;

    void draw() const;
};
