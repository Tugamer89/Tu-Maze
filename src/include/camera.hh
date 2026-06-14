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

    // Accurate Circle vs AABB collision detection to prevent corner snagging
    bool checkCollision(const glm::vec3& pos, const Maze& maze) const {
        float offsetX = (static_cast<float>(maze.width) * Maze::CELL_SIZE) * 0.5f;
        float offsetZ = (static_cast<float>(maze.height) * Maze::CELL_SIZE) * 0.5f;

        // Current grid cell estimation
        auto cellX = static_cast<int>(std::floor(pos.x + offsetX) / Maze::CELL_SIZE);
        auto cellY = static_cast<int>(std::floor(pos.z + offsetZ) / Maze::CELL_SIZE);

        glm::vec2 center(pos.x, pos.z);

        // true if player AABB intersects with wall AABB
        auto checkAABB = [&](float minX, float maxX, float minZ, float maxZ) {
            bool intersectX = (pos.x + collisionRadius > minX) && (pos.x - collisionRadius < maxX);
            bool intersectZ = (pos.z + collisionRadius > minZ) && (pos.z - collisionRadius < maxZ);
            return intersectX && intersectZ;
        };

        float halfCell = Maze::CELL_SIZE * 0.5f;
        float halfWall = Maze::WALL_THICKNESS * 0.5f;
        float outer = halfCell + halfWall;
        float inner = halfCell - halfWall;

        // Check a 3x3 grid around the player to guarantee we catch corner walls
        for (int y = std::max(0, cellY - 1); y <= std::min(maze.height - 1, cellY + 1); ++y) {
            for (int x = std::max(0, cellX - 1); x <= std::min(maze.width - 1, cellX + 1); ++x) {
                const Cell& cell = maze.grid[y * maze.width + x];

                float cx = (static_cast<float>(x) * Maze::CELL_SIZE) - offsetX + halfCell;
                float cz = (static_cast<float>(y) * Maze::CELL_SIZE) - offsetZ + halfCell;

                // Wall bounds
                if (cell.wallTop && y == 0 &&
                    checkAABB(cx - outer, cx + outer, cz - outer, cz - inner))
                    return true;
                if (cell.wallBottom && checkAABB(cx - outer, cx + outer, cz + inner, cz + outer))
                    return true;
                if (cell.wallLeft && x == 0 &&
                    checkAABB(cx - outer, cx - inner, cz - outer, cz + outer))
                    return true;
                if (cell.wallRight && checkAABB(cx + inner, cx + outer, cz - outer, cz + outer))
                    return true;
            }
        }

        // Restrict to absolute outer maze boundaries
        if (pos.x - collisionRadius < -offsetX || pos.x + collisionRadius > offsetX ||
            pos.z - collisionRadius < -offsetZ || pos.z + collisionRadius > offsetZ) {
            return true;
        }

        return false;
    }

   public:
    explicit Camera(const Shaders& shaders) {
        updateCameraVectors();
        locations(shaders);
    }

    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getFront() const { return front; }
    float getYaw() const { return yaw; }

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

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
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
