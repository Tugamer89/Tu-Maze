#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <thread>
#include <atomic>

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

////////////////////
// Loading Screen //
////////////////////

struct GameAssets {
    // Texture
    std::unique_ptr<Texture> wallDiff, wallNorm, wallRough;  // NOSONAR
    std::unique_ptr<Texture> floorDiff, floorNorm, floorRough;  // NOSONAR
    // Meshes
    std::unique_ptr<GPUMesh> floorMesh, wallMesh;  // NOSONAR
    // Materiali
    std::optional<Material> wallMat, floorMat;  // NOSONAR
    // Gameplay
    std::unique_ptr<Maze> maze;
};

std::string nextLoadingStep(int step, GameAssets& assets, Scene& scene) {
    switch(step) {
    case 0:
        assets.wallDiff = std::make_unique<Texture>("resources/textures/mossy_brick_diff_4k.jpg", true);
        return "mossy bricks texture";
    case 1:
        assets.wallNorm = std::make_unique<Texture>("resources/textures/mossy_brick_nor_gl_4k.png");
        return "mossy bricks normals map";
    case 2:
        assets.wallRough = std::make_unique<Texture>("resources/textures/mossy_brick_rough_4k.png");
        return "mossy bricks roughness map";
    case 3:
        assets.floorDiff = std::make_unique<Texture>("resources/textures/cobblestone_pavement_diff_4k.jpg", true);
        return "cobblestone pavement texture";
    case 4:
        assets.floorNorm = std::make_unique<Texture>("resources/textures/cobblestone_pavement_nor_gl_4k.png");
        return "cobblestone pavement normals map";
    case 5:
        assets.floorRough = std::make_unique<Texture>("resources/textures/cobblestone_pavement_rough_4k.png");
        return "cobblestone pavement roughness map";
    case 6:
        assets.wallMesh = std::make_unique<GPUMesh>(wall_mesh);
        return "wall mesh";
    case 7:
        assets.floorMesh = std::make_unique<GPUMesh>(floor_mesh);
        return "floor mesh";
    case 8:
        assets.wallMat = Material{assets.wallDiff.get(), assets.wallNorm.get(), assets.wallRough.get()};
        return "wall material";
    case 9:
        assets.floorMat = Material{assets.floorDiff.get(), assets.floorNorm.get(), assets.floorRough.get()};
        return "floor material";
    case 10:
        assets.maze = std::make_unique<Maze>(10, 10);
        return "random maze";
    case 11: 
        {
            Node mazeNode = assets.maze->populateSceneNode(assets.floorMesh.get(), assets.wallMesh.get(), *assets.wallMat, *assets.floorMat);
            scene.root.children.push_back(mazeNode);
        }
        return "scene";

    default:
        return "something";
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

    Shaders shaders(flat_vert, flat_frag);
    shaders.use();

    Scene scene(shaders);

    GameAssets assets;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //// Main Loop ////

    const int TOTAL_STEPS = 12;
    int step = 0;
    
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

        if (step < TOTAL_STEPS) {
            std::string what = nextLoadingStep(step, assets, scene);
            step++;

            float progress = static_cast<float>(step) / static_cast<float>(TOTAL_STEPS);
            gui.renderLoading(window, what, progress);

        } else {
            // --- OpenGl rendering ---
            scene.draw();

            // --- ImGui rendering ---
            gui.render(scene);
        }

        window.display();
    }

    window.close();

    return 0;
}
