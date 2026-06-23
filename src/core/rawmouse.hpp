#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

class RawMouse {
   private:
#ifdef __APPLE__
    bool first_call = true;
#else
    sf::Vector2f accumulated_delta{0.f, 0.f};
#endif

   public:
    RawMouse() = default;

    // Accumulates the raw, unaccelerated delta from a MouseMovedRaw event.
    void event(const sf::Event::MouseMovedRaw& e);

    // Returns the raw, unaccelerated hardware delta since the last call.
    sf::Vector2f delta();

    // Set mouse position in screen coordinates
    void setPosition(sf::Vector2i position) const;
};
