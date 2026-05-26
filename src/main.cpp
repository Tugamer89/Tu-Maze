#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>

int main() {
    const int window_width = 800;
    const int window_height = 600;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition((desktop.size.x - window_width) / 2,
                                (desktop.size.y - window_height) / 2);

    sf::RenderWindow window(sf::VideoMode({window_width, window_height}), "SFML 3.0 & GLM Project Template");
    window.setPosition(centerPosition);
    window.setFramerateLimit(60);

    glm::vec3 baselineVector(1.0f, 2.0f, 3.0f);
    glm::vec3 scaledVector = baselineVector * 3.0f;
    std::cout << "[GLM] System check passed! Scaled vector check: "
              << scaledVector.x << ", " << scaledVector.y << ", " << scaledVector.z << std::endl;

    // Define a baseline visual shape (Circle) for the application render loop
    sf::CircleShape centralShape(80.f);
    centralShape.setFillColor(sf::Color::Green);
    centralShape.setOrigin({80.f, 80.f});
    centralShape.setPosition({400.f, 300.f});

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
                    window.close();
                }
            }
        }

        // Draw and update frame
        window.clear(sf::Color(25, 25, 25)); // Dark background
        window.draw(centralShape);
        window.display();
    }

    return 0;
}
