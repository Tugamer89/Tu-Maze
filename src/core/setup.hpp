#pragma once

#include <SFML/Window.hpp>

class Setup {
   private:
    sf::Window window;

   public:
    Setup();

    sf::Window& getWindow() { return window; }
    [[nodiscard]] const sf::Window& getWindow() const { return window; }
};
