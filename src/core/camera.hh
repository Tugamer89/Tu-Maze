#pragma once

#include <algorithm>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "game/maze.hh"
#include "render/hotshaders.hh"

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
    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        newFront.y = static_cast<float>(sin(glm::radians(pitch)));
        newFront.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    // Extracts the 6 bounding planes of the camera's frustum from the View-Projection matrix
    // Used heavily for CPU-side culling to prevent the GPU from drawing unseen geometry
    void updateFrustumPlanes() {
        glm::vec4 row0(vp[0][0], vp[1][0], vp[2][0], vp[3][0]);
        glm::vec4 row1(vp[0][1], vp[1][1], vp[2][1], vp[3][1]);
        glm::vec4 row2(vp[0][2], vp[1][2], vp[2][2], vp[3][2]);
        glm::vec4 row3(vp[0][3], vp[1][3], vp[2][3], vp[3][3]);

        frustumPlanes[0] = row3 + row0;  // Left
        frustumPlanes[1] = row3 - row0;  // Right
        frustumPlanes[2] = row3 + row1;  // Bottom
        frustumPlanes[3] = row3 - row1;  // Top
        frustumPlanes[4] = row3 + row2;  // Near
        frustumPlanes[5] = row3 - row2;  // Far

        // Normalize the planes to allow accurate distance calculations
        for (auto& plane : frustumPlanes) {
            float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }

    [[nodiscard]] bool isAABBIntersecting(const glm::vec3& pos, float minX, float maxX, float minZ,
                                          float maxZ) const {
        return (pos.x + collisionRadius > minX) && (pos.x - collisionRadius < maxX) &&
               (pos.z + collisionRadius > minZ) && (pos.z - collisionRadius < maxZ);
    }

    // Evaluates collision against the physical boundaries of a specific maze cell
    [[nodiscard]] bool checkCellWalls(const glm::vec3& pos, const Cell& cell, int x, int y,
                                      float cx, float cz) const {
        constexpr float halfCell = Maze::CELL_SIZE * 0.5f;
        constexpr float halfWall = Maze::WALL_THICKNESS * 0.5f;
        constexpr float outer = halfCell + halfWall;
        constexpr float inner = halfCell - halfWall;

        if (cell.wallTop && y == 0 &&
            isAABBIntersecting(pos, cx - outer, cx + outer, cz - outer, cz - inner))
            return true;
        if (cell.wallBottom &&
            isAABBIntersecting(pos, cx - outer, cx + outer, cz + inner, cz + outer))
            return true;
        if (cell.wallLeft && x == 0 &&
            isAABBIntersecting(pos, cx - outer, cx - inner, cz - outer, cz + outer))
            return true;
        if (cell.wallRight &&
            isAABBIntersecting(pos, cx + inner, cx + outer, cz - outer, cz + outer))
            return true;

        return false;
    }

    // High-performance collision detection using spatial partitioning.
    // Instead of checking every wall in the maze, it only checks the cell the player
    // is currently in, plus adjacent cells if the player is near a boundary.
    [[nodiscard]] bool checkCollision(const glm::vec3& pos, const Maze& maze) const {
        float offsetX = (static_cast<float>(maze.getWidth()) * Maze::CELL_SIZE) * 0.5f;
        float offsetZ = (static_cast<float>(maze.getHeight()) * Maze::CELL_SIZE) * 0.5f;

        float gridPosX = pos.x + offsetX;
        float gridPosZ = pos.z + offsetZ;

        auto cellX = static_cast<int>(std::floor(gridPosX / Maze::CELL_SIZE));
        auto cellY = static_cast<int>(std::floor(gridPosZ / Maze::CELL_SIZE));

        // Calculate sub-cell fractional position to determine boundary proximity
        float localX = gridPosX - (static_cast<float>(cellX) * Maze::CELL_SIZE);
        float localZ = gridPosZ - (static_cast<float>(cellY) * Maze::CELL_SIZE);

        float halfCell = Maze::CELL_SIZE * 0.5f;
        float halfWall = Maze::WALL_THICKNESS * 0.5f;
        float threshold = collisionRadius + halfWall;

        // Create a minimal bounding box of cells to check
        int minX = std::max(0, (localX < threshold) ? cellX - 1 : cellX);
        int maxX = std::min(maze.getWidth() - 1,
                            (localX > Maze::CELL_SIZE - threshold) ? cellX + 1 : cellX);
        int minZ = std::max(0, (localZ < threshold) ? cellY - 1 : cellY);
        int maxZ = std::min(maze.getHeight() - 1,
                            (localZ > Maze::CELL_SIZE - threshold) ? cellY + 1 : cellY);

        for (int y = minZ; y <= maxZ; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                const Cell& cell = maze.getCellAt(x, y);
                float cx = (static_cast<float>(x) * Maze::CELL_SIZE) - offsetX + halfCell;
                float cz = (static_cast<float>(y) * Maze::CELL_SIZE) - offsetZ + halfCell;

                if (checkCellWalls(pos, cell, x, y, cx, cz)) {
                    return true;
                }
            }
        }

        // Restrict player to absolute outer maze boundaries
        return (pos.x - collisionRadius < -offsetX || pos.x + collisionRadius > offsetX ||
                pos.z - collisionRadius < -offsetZ || pos.z + collisionRadius > offsetZ);
    }

   public:
    explicit Camera(const Shaders& shaders) {
        updateCameraVectors();
        locations(shaders);
    }

    [[nodiscard]] const glm::mat4& getViewMatrix() const { return v; }
    [[nodiscard]] const glm::mat4& getInverseViewMatrix() const { return inv_v; }
    [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return vp; }
    [[nodiscard]] const std::array<glm::vec4, 6>& getFrustumPlanes() const { return frustumPlanes; }
    [[nodiscard]] const glm::vec3& getPosition() const { return position; }
    [[nodiscard]] const glm::vec3& getFront() const { return front; }
    [[nodiscard]] float getYaw() const { return yaw; }

    void setPosition(const glm::vec3& pos) {
        position = pos;
        projection();
    }

    void setYaw(float p_yaw) {
        yaw = p_yaw;
        updateCameraVectors();
        projection();
    }

    void setPitch(float p_pitch) {
        pitch = p_pitch;
        updateCameraVectors();
        projection();
    }

    void setAspectRatio(float ratio) {
        aspectRatio = ratio;
        projection();
    }

    void setFov(float newFov) {
        baseFov = newFov;
        currentRenderFov = newFov;
        projection();
    }

    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setBobbingAmount(float amount) { bobbingAmount = amount; }

    void locations(const Shaders& shaders) {
        camera_pos_loc = glGetUniformLocation(shaders.getProgram(), "camera_pos");
    }

    // Main camera logic pump (handles input, physics, and view interpolation)
    bool update(sf::Time dt, const Maze& maze) {
        float dt_secs = dt.asSeconds();
        float currentSpeed = baseMovementSpeed;

        bool isSprinting = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                           sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

        if (isSprinting) {
            currentSpeed *= sprintMultiplier;
        }

        float velocity = currentSpeed * dt_secs;

        // Flatten vectors to prevent flying when looking up/down
        glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 flatRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

        glm::vec3 movement(0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movement += flatFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement -= flatFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movement -= flatRight;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement += flatRight;

        bool isMoving = glm::length(movement) > 0.0f;
        bool needsProjectionUpdate = false;

        // Process movement using separated axis theorem (sliding collision)
        if (isMoving) {
            movement = glm::normalize(movement) * velocity;

            glm::vec3 nextPosX = position;
            nextPosX.x += movement.x;
            if (!checkCollision(nextPosX, maze)) {
                position.x = nextPosX.x;
            }

            glm::vec3 nextPosZ = position;
            nextPosZ.z += movement.z;
            if (!checkCollision(nextPosZ, maze)) {
                position.z = nextPosZ.z;
            }

            needsProjectionUpdate = true;
        }

        // Apply smooth head-bobbing using a sine wave synced to movement
        float targetAmplitude = isMoving ? bobbingAmount : 0.0f;
        currentBobbingAmplitude += (targetAmplitude - currentBobbingAmplitude) * 10.0f * dt_secs;

        if (isMoving) {
            float speedMult = isSprinting ? sprintMultiplier : 1.0f;
            walkTimer += dt_secs * bobbingSpeed * speedMult;
            walkTimer = std::fmod(walkTimer, 2.0f * glm::pi<float>());
        }

        float targetY = baseHeight + std::sin(walkTimer) * currentBobbingAmplitude;

        if (std::abs(position.y - targetY) > 0.001f) {
            position.y = targetY;
            needsProjectionUpdate = true;
        }

        // Apply dynamic field-of-view when sprinting to simulate speed
        float targetFov = (isSprinting && isMoving) ? (baseFov + sprintFovOffset) : baseFov;

        if (std::abs(currentRenderFov - targetFov) > 0.05f) {
            currentRenderFov += (targetFov - currentRenderFov) * fovTransitionSpeed * dt_secs;
            needsProjectionUpdate = true;
        } else if (currentRenderFov != targetFov) {
            currentRenderFov = targetFov;
            needsProjectionUpdate = true;
        }

        if (needsProjectionUpdate) {
            projection();
            return true;
        }

        return false;
    }

    void processMouseMovement(float xoffset, float yoffset) {
        if (std::abs(xoffset) < 0.001f && std::abs(yoffset) < 0.001f) return;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp pitch to prevent the camera from flipping backwards
        pitch = std::clamp(pitch, -89.0f, 89.0f);

        updateCameraVectors();
        projection();
    }

    void projection() {
        v = glm::lookAt(position, position + front, up);
        inv_v = glm::inverse(v);

        glm::mat4 pr = glm::perspective(glm::radians(currentRenderFov), aspectRatio, 0.1f, 100.0f);

        vp = pr * v;
        updateFrustumPlanes();

        glUniform3fv(camera_pos_loc, 1, &position[0]);
    }
};
