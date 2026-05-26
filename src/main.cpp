#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <vector>

///////////////////////////////
// CONSTANTS & CONFIGURATION //
///////////////////////////////

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr float FOV_SCALE = 400.0f;      // Controls how large the projection appears
constexpr float CAMERA_DISTANCE = 3.0f;  // Distance from the camera to the cube on the Z axis

constexpr float ROTATION_SPEED_Y = 1.2f;  // Radians per second

/////////////////////////
// AUXILIARY FUNCTIONS //
/////////////////////////

glm::vec3 animatePoint(const glm::vec3& point, float time) {
    float angleY = time * ROTATION_SPEED_Y;

    float s = std::sin(angleY);
    float c = std::cos(angleY);

    float rotY_x = point.x * c - point.z * s;
    float rotY_z = point.x * s + point.z * c;
    float rotY_y = point.y;

    return glm::vec3(rotY_x, rotY_y, rotY_z);
}

sf::Vector2f projectPoint(const glm::vec3& point3D) {
    float z = point3D.z + CAMERA_DISTANCE;

    z = std::max(z, 0.0001f);

    // Perspective projection formula
    float projectedX = point3D.x / z;
    float projectedY = point3D.y / z;

    // Scale and center the coordinates to the window resolution
    float screenX = (projectedX * FOV_SCALE) + (WINDOW_WIDTH / 2.0f);
    float screenY = (projectedY * FOV_SCALE) + (WINDOW_HEIGHT / 2.0f);

    return sf::Vector2f(screenX, screenY);
}

//////////
// MAIN //
//////////

int main() {
    // Initialize Window
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition((desktop.size.x - WINDOW_WIDTH) / 2,
                                (desktop.size.y - WINDOW_HEIGHT) / 2);

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                            "SFML 3.0 - 3D Cube Projection");
    window.setPosition(centerPosition);
    window.setFramerateLimit(60);

    std::cout << "[Template] Starting 3D Cube projection rendering loop!" << std::endl;

    const std::vector<glm::vec3> cubeVertices = {
        {-1.f, -1.f, -1.f}, {1.f, -1.f, -1.f}, {1.f, 1.f, -1.f}, {-1.f, 1.f, -1.f},  // Front Face
        {-1.f, -1.f, 1.f},  {1.f, -1.f, 1.f},  {1.f, 1.f, 1.f},  {-1.f, 1.f, 1.f}    // Back Face
    };

    const std::vector<std::pair<int, int>> cubeEdges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // Front face edges
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // Back face edges
        {0, 4}, {1, 5}, {2, 6}, {3, 7}   // Connecting depth edges
    };

    sf::Clock clock;

    // Main Game Loop
    while (window.isOpen()) {
        // Event Handling
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                }
            }
        }

        // Update Logic
        float currentTime = clock.getElapsedTime().asSeconds();
        std::vector<sf::Vector2f> screenVertices;
        screenVertices.reserve(cubeVertices.size());

        // Animate and Project each vertex
        for (const auto& vertex : cubeVertices) {
            glm::vec3 animatedPoint = animatePoint(vertex, currentTime);
            sf::Vector2f projectedPoint = projectPoint(animatedPoint);
            screenVertices.push_back(projectedPoint);
        }

        // Render
        window.clear(sf::Color(30, 30, 35));  // Dark gray background

        for (const auto& [first, second] : cubeEdges) {
            sf::Vector2f p1 = screenVertices[first];
            sf::Vector2f p2 = screenVertices[second];

            sf::Vector2f midPoint = (p1 + p2) / 2.0f;

            std::array<sf::Vertex, 4> lines = {
                sf::Vertex{p1, sf::Color::Cyan},
                sf::Vertex{midPoint, sf::Color::Magenta},

                sf::Vertex{midPoint, sf::Color::Magenta},
                sf::Vertex{p2, sf::Color::Cyan},
            };

            window.draw(lines.data(), lines.size(), sf::PrimitiveType::Lines);
        }

        window.display();
    }

    return 0;
}
