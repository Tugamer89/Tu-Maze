#ifndef CAMERA_HH
#define CAMERA_HH

#include <algorithm>
#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "hotshaders.hh"
#include "maze.hh"

class Camera {
   public:
    glm::mat4 v;
    glm::mat4 inv_v;
    glm::mat4 vp;
    std::array<glm::vec4, 6> frustumPlanes;

   private:
    GLint camera_pos_loc;

    // Camera Attributes
    glm::vec3 position = {0.0f, 0.5f, 0.0f};  // Start at height 0.5 (inside the maze)
    glm::vec3 front = {0.0f, 0.0f, -1.0f};    // Look towards -Z
    glm::vec3 up = {0.0f, 1.0f, 0.0f};        // Up is +Y
    glm::vec3 right;
    glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};  // World up vector

    // Euler angles
    float yaw = -90.0f;
    float pitch = 0.0f;

    // Camera options
    float baseMovementSpeed = 2.5f;  // Units per second
    float sprintMultiplier = 1.6f;
    float mouseSensitivity = 0.2f;
    float fov = 60.0f;
    float aspectRatio = 1.0f;

    // Player collision size
    float collisionRadius = 0.20f;

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        newFront.y = static_cast<float>(sin(glm::radians(pitch)));
        newFront.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void updateFrustumPlanes() {
        // Extract matrix rows
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

        // Normalize the planes
        for (auto& plane : frustumPlanes) {
            float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }

    // Helper for collision checking
    bool isAABBIntersecting(const glm::vec3& pos, float minX, float maxX, float minZ,
                            float maxZ) const {
        return (pos.x + collisionRadius > minX) && (pos.x - collisionRadius < maxX) &&
               (pos.z + collisionRadius > minZ) && (pos.z - collisionRadius < maxZ);
    }

    // Helper to evaluate specific walls of a cell
    bool checkCellWalls(const glm::vec3& pos, const Cell& cell, int x, int y, float cx,
                        float cz) const {
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

    // Accurate Circle vs AABB collision detection optimized by spatial partitioning
    bool checkCollision(const glm::vec3& pos, const Maze& maze) const {
        float offsetX = (static_cast<float>(maze.width) * Maze::CELL_SIZE) * 0.5f;
        float offsetZ = (static_cast<float>(maze.height) * Maze::CELL_SIZE) * 0.5f;

        float gridPosX = pos.x + offsetX;
        float gridPosZ = pos.z + offsetZ;

        auto cellX = static_cast<int>(std::floor(gridPosX / Maze::CELL_SIZE));
        auto cellY = static_cast<int>(std::floor(gridPosZ / Maze::CELL_SIZE));

        // Fractional position inside the current cell [0.0, 1.0)
        float localX = gridPosX - (static_cast<float>(cellX) * Maze::CELL_SIZE);
        float localZ = gridPosZ - (static_cast<float>(cellY) * Maze::CELL_SIZE);

        float halfCell = Maze::CELL_SIZE * 0.5f;
        float halfWall = Maze::WALL_THICKNESS * 0.5f;

        // Check only adjacent cells if the player is dangerously close to the boundaries
        float threshold = collisionRadius + halfWall;

        int minX = std::max(0, (localX < threshold) ? cellX - 1 : cellX);
        int maxX =
            std::min(maze.width - 1, (localX > Maze::CELL_SIZE - threshold) ? cellX + 1 : cellX);
        int minZ = std::max(0, (localZ < threshold) ? cellY - 1 : cellY);
        int maxZ =
            std::min(maze.height - 1, (localZ > Maze::CELL_SIZE - threshold) ? cellY + 1 : cellY);

        for (int y = minZ; y <= maxZ; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                const Cell& cell = maze.grid[y * maze.width + x];
                float cx = (static_cast<float>(x) * Maze::CELL_SIZE) - offsetX + halfCell;
                float cz = (static_cast<float>(y) * Maze::CELL_SIZE) - offsetZ + halfCell;

                if (checkCellWalls(pos, cell, x, y, cx, cz)) {
                    return true;
                }
            }
        }

        // Restrict to absolute outer maze boundaries
        return (pos.x - collisionRadius < -offsetX || pos.x + collisionRadius > offsetX ||
                pos.z - collisionRadius < -offsetZ || pos.z + collisionRadius > offsetZ);
    }

   public:
    explicit Camera(const Shaders& shaders) {
        updateCameraVectors();
        locations(shaders);
    }

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getFront() const { return front; }
    float getYaw() const { return yaw; }

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

    void locations(const Shaders& shaders) {
        camera_pos_loc = glGetUniformLocation(shaders.program, "camera_pos");
    }

    void setAspectRatio(float ratio) {
        aspectRatio = ratio;
        projection();
    }

    void setFov(float newFov) {
        fov = newFov;
        projection();
    };

    bool update(sf::Time dt, const Maze& maze) {
        float dt_secs = dt.asSeconds();
        float currentSpeed = baseMovementSpeed;

        // Sprint
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
            currentSpeed *= sprintMultiplier;
        }

        float velocity = currentSpeed * dt_secs;

        glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 flatRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

        glm::vec3 movement(0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movement += flatFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement -= flatFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movement -= flatRight;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement += flatRight;

        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement) * velocity;

            // X-Axis separated collision check
            glm::vec3 nextPosX = position;
            nextPosX.x += movement.x;
            if (!checkCollision(nextPosX, maze)) {
                position.x = nextPosX.x;
            }

            // Z-Axis separated collision check
            glm::vec3 nextPosZ = position;
            nextPosZ.z += movement.z;
            if (!checkCollision(nextPosZ, maze)) {
                position.z = nextPosZ.z;
            }

            projection();
            return true;
        }

        return false;
    }

    void processMouseMovement(float xoffset, float yoffset) {
        if (xoffset == 0 && yoffset == 0) return;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        pitch = std::clamp(pitch, -89.0f, 89.0f);

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();

        // Update the view and projection matrices
        projection();
    }

    void projection() {
        // Build View Matrix
        v = glm::lookAt(position, position + front, up);
        inv_v = glm::inverse(v);

        // Build Projection Matrix
        glm::mat4 pr = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);

        // Compute VP matrix
        vp = pr * v;

        // Update the frustum planes for culling
        updateFrustumPlanes();

        // Update the camera position uniform in the shader
        glUniform3fv(camera_pos_loc, 1, &position[0]);
    }
};

#endif
