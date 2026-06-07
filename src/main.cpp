#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <iostream>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "include/camera.hh"
#include "include/gpumesh.hh"
#include "include/gui.hh"
#include "include/hotshaders.hh"
#include "include/lights.hh"
#include "include/matrices.hh"
#include "include/maze.hh"
#include "include/mesh.hh"
#include "include/node.hh"
#include "include/scene.hh"
#include "include/setup.hh"

///////////////
// Constants //
///////////////

const std::string phong_vert = "resources/shaders/shader_phong.vert";
const std::string phong_frag = "resources/shaders/shader_phong.frag";

const std::string flat_vert = "resources/shaders/shader_flat.vert";
const std::string flat_frag = "resources/shaders/shader_flat.frag";

const std::string floor_mesh = "resources/meshes/floor.off";
const std::string wall_mesh = "resources/meshes/wall.off";

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, Scene& scene, bool& running) {
    switch (key.scancode) {
        using enum sf::Keyboard::Scancode;

        case P:
            shaders.reload(phong_vert, phong_frag);
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case F:
            shaders.reload(flat_vert, flat_frag);
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            return;
        case Escape:
            running = false;
            return;
        default:
            return;
    }
}

void handle(const sf::Event::MouseMoved& mouse, Scene& scene) {
    auto x = static_cast<float>(mouse.position.x);
    auto y = static_cast<float>(mouse.position.y);
    static float prev_x = 0.f;
    static float prev_y = 0.f;

    float dx = x - prev_x;
    float dy = y - prev_y;

    prev_x = x;
    prev_y = y;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        scene.camera.drag(dx, dy);
        scene.camera.projection();
        scene.lights.position(scene.camera.inv_v);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt)) {
        scene.camera.dolly(dy);
        scene.camera.projection();
        scene.lights.position(scene.camera.inv_v);
    }
}

//////////
// Main //
//////////

int main(int argc, char* argv[]) {
    //// Startup ////

    Setup setup;
    sf::Window& window = setup.window;

    Gui gui(window);

    Maze maze(20, 20);
    GPUMesh floorMesh(floor_mesh);
    GPUMesh wallMesh(wall_mesh);

    Shaders shaders(flat_vert, flat_frag);
    shaders.use();

    Scene scene(shaders);

    //// Texture Loading ////
    // Wall
    Texture wallDiff("resources/textures/mossy_brick_diff_4k.jpg", true);
    Texture wallNorm("resources/textures/mossy_brick_nor_gl_4k.png");
    Texture wallRough("resources/textures/mossy_brick_rough_4k.png");
    
    Material wallMaterial;
    wallMaterial.diffuse = &wallDiff;
    wallMaterial.normal = &wallNorm;
    wallMaterial.roughness = &wallRough;

    // Floor
    Texture floorDiff("resources/textures/cobblestone_pavement_diff_4k.jpg", true);
    Texture floorNorm("resources/textures/cobblestone_pavement_nor_gl_4k.png");
    Texture floorRough("resources/textures/cobblestone_pavement_rough_4k.png");
    
    Material floorMaterial;
    floorMaterial.diffuse = &floorDiff;
    floorMaterial.normal = &floorNorm;
    floorMaterial.roughness = &floorRough;

    Node mazeNode = maze.populateSceneNode(&floorMesh, &wallMesh, wallMaterial, floorMaterial);
    scene.root.children.push_back(mazeNode);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    //// Main Loop ////

    sf::Clock deltaClock;
    bool running = true;

    while (running) {
        while (const std::optional event = window.pollEvent()) {
            gui.process_event(window, *event);

            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
                glViewport(0, 0, resized->size.x, resized->size.y);
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>();
                     key_pressed && !gui.wants_capture_keyboard()) {
                handle(*key_pressed, shaders, scene, running);
            } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>();
                       mouse && !gui.wants_capture_mouse()) {
                handle(*mouse, scene);
            }
        }

        gui.update(window, deltaClock.restart());

        // --- OpenGl rendering ---
        scene.draw();

        // --- ImGui rendering ---
        gui.render(scene);

        window.display();
    }

    window.close();

    return 0;
}
