#pragma once

#include <SFML/System/Time.hpp>
#include <array>
#include <glm/mat4x4.hpp>

#include "game/maze.hpp"
#include "glad/gl.h"
#include "render/hotshaders.hpp"

class Camera {
   private:
    glm::mat4 v;      // View Matrix
    glm::mat4 inv_v;  // Inverse View Matrix (useful for billboard lighting)
    glm::mat4 vp;     // View-Projection Matrix (sent directly to shaders)
    std::array<glm::vec4, 6> frustumPlanes{};

    GLint camera_pos_loc = -1;

    glm::vec3 position = {0.0f, 0.5f, 0.0f};
    glm::vec3 front = {0.0f, 0.0f, -1.0f};
    glm::vec3 up = {0.0f, 1.0f, 0.0f};
    glm::vec3 right = {1.0f, 0.0f, 0.0f};
    glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};

    float yaw = -90.0f;
    float pitch = 0.0f;

    float baseMovementSpeed = 2.5f;
    float sprintMultiplier = 1.6f;
    float mouseSensitivity = 0.2f;
    float aspectRatio = 1.0f;

    float baseFov = 60.0f;
    float currentRenderFov = 60.0f;
    float sprintFovOffset = 12.0f;
    float fovTransitionSpeed = 8.0f;

    float baseHeight = 0.5f;
    float walkTimer = 0.0f;
    float bobbingSpeed = 12.0f;
    float bobbingAmount = 0.02f;
    float currentBobbingAmplitude = 0.0f;

    float collisionRadius = 0.20f;

    // Recalculates the directional vectors based on the current yaw and pitch
    void updateCameraVectors();

    // Extracts the 6 bounding planes of the camera's frustum from the View-Projection matrix
    // Used heavily for CPU-side culling to prevent the GPU from drawing unseen geometry
    void updateFrustumPlanes();

    [[nodiscard]] bool isAABBIntersecting(const glm::vec3& pos, float minX, float maxX, float minZ,
                                          float maxZ) const;

    // Evaluates collision against the physical boundaries of a specific maze cell
    [[nodiscard]] bool checkCellWalls(const glm::vec3& pos, const Cell& cell, int x, int y,
                                      float cx, float cz) const;

    // High-performance collision detection using spatial partitioning.
    // Instead of checking every wall in the maze, it only checks the cell the player
    // is currently in, plus adjacent cells if the player is near a boundary.
    [[nodiscard]] bool checkCollision(const glm::vec3& pos, const Maze& maze) const;

   public:
    explicit Camera(const Shaders& shaders);

    [[nodiscard]] const glm::mat4& getViewMatrix() const { return v; }
    [[nodiscard]] const glm::mat4& getInverseViewMatrix() const { return inv_v; }
    [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return vp; }
    [[nodiscard]] const std::array<glm::vec4, 6>& getFrustumPlanes() const { return frustumPlanes; }
    [[nodiscard]] const glm::vec3& getPosition() const { return position; }
    [[nodiscard]] const glm::vec3& getFront() const { return front; }
    [[nodiscard]] float getYaw() const { return yaw; }

    void setPosition(const glm::vec3& pos);
    void setYaw(float p_yaw);
    void setPitch(float p_pitch);
    void setAspectRatio(float ratio);
    void setFov(float newFov);

    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setBobbingAmount(float amount) { bobbingAmount = amount; }

    void locations(const Shaders& shaders);

    // Main camera logic pump (handles input, physics, and view interpolation)
    bool update(sf::Time dt, const Maze& maze);

    void processMouseMovement(float xoffset, float yoffset);
    void projection();
};
