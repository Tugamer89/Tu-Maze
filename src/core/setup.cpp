#include "core/setup.hpp"

#include <iostream>

#include "core/exceptions.hpp"
#include "glad/gl.h"
#include "ui/gui.hpp"

Setup::Setup() {
    // Request Core profile (mandatory for Mac and modern pipeline rendering)
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = GuiState::getSavedMSAA();
    settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    settings.majorVersion = 4;
    settings.minorVersion = 1;

    const unsigned int window_width = 800;
    const unsigned int window_height = 800;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2i centerPosition(static_cast<int>(desktop.size.x - window_width) / 2,
                                static_cast<int>(desktop.size.y - window_height) / 2);

    window.create(sf::VideoMode({window_width, window_height}), "Tu Maze", sf::Style::Default,
                  sf::State::Windowed, settings);
    window.setPosition(centerPosition);

    if (!window.setActive(true)) {
        throw exceptions::EngineSetupException("Failure: error during SFML OpenGL Activation.");
    }

    sf::ContextSettings gotten = window.getSettings();

    std::cout << "Depth bits: " << gotten.depthBits << std::endl;
    std::cout << "Stencil bits: " << gotten.stencilBits << std::endl;
    std::cout << "Antialiasing level: " << gotten.antiAliasingLevel << std::endl;
    std::cout << "SFML GL version: " << gotten.majorVersion << "." << gotten.minorVersion
              << std::endl;

    // Initialize GLAD to map hardware-specific OpenGL function pointers
    int version = gladLoadGL(sf::Context::getFunction);
    if (!version) {
        throw exceptions::EngineSetupException("Failure: error during GLAD loading.");
    }
    std::cout << "GLAD GL version: " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    // Core OpenGL Settings
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
