#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "hotshaders.hh"

enum class Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
   public:
    glm::mat4 v;
    glm::mat4 inv_v;
    glm::mat4 vp;

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
    float movementSpeed = 2.5f;  // Units per second
    float mouseSensitivity = 0.2f;
    float fov = 45.0f;
    float aspectRatio = 1.0f;

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
        newFront.y = static_cast<float>(sin(glm::radians(pitch)));
        newFront.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

        front = glm::normalize(newFront);
        // Also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void move(Camera_Movement direction, float deltaTime) {
        using enum Camera_Movement;

        float velocity = movementSpeed * deltaTime;

        // Project the front vector onto the XZ plane to prevent flying
        glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        glm::vec3 flatRight = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

        if (direction == FORWARD) position += flatFront * velocity;
        if (direction == BACKWARD) position -= flatFront * velocity;
        if (direction == LEFT) position -= flatRight * velocity;
        if (direction == RIGHT) position += flatRight * velocity;
    }

   public:
    explicit Camera(const Shaders& shaders) {
        updateCameraVectors();
        locations(shaders);
    }

    void locations(const Shaders& shaders) {
        camera_pos_loc = glGetUniformLocation(shaders.program, "camera_pos");
    }

    void setAspectRatio(float ratio) { aspectRatio = ratio; }

    bool update(sf::Time dt) {
        using enum Camera_Movement;

        bool moved = false;
        float dt_secs = dt.asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            move(FORWARD, dt_secs);
            moved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            move(BACKWARD, dt_secs);
            moved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            move(LEFT, dt_secs);
            moved = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            move(RIGHT, dt_secs);
            moved = true;
        }

        return moved;
    }

    void processMouseMovement(float xoffset, float yoffset) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        pitch = std::clamp(pitch, -89.0f, 89.0f);

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void projection() {
        // Build View Matrix
        v = glm::lookAt(position, position + front, up);
        inv_v = glm::inverse(v);

        // Build Projection Matrix
        glm::mat4 pr = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);

        // Compute VP matrix
        vp = pr * v;

        // Update the camera position uniform in the shader
        glUniform3fv(camera_pos_loc, 1, &position[0]);
    }
};

#endif
