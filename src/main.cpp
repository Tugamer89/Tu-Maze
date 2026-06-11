#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <atomic>
#include <iostream>
#include <thread>

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
#include "include/minimap.hh"
#include "include/node.hh"
#include "include/scene.hh"
#include "include/setup.hh"

///////////////
// Constants //
///////////////

const std::string phong_vert = "resources/shaders/phong.vert";
const std::string phong_frag = "resources/shaders/phong.frag";

const std::string flat_vert = "resources/shaders/flat.vert";
const std::string flat_frag = "resources/shaders/flat.frag";

const std::string minimap_vert = "resources/shaders/minimap.vert";
const std::string minimap_frag = "resources/shaders/minimap.frag";

const std::string floor_mesh = "resources/meshes/floor.off";
const std::string wall_mesh = "resources/meshes/wall.off";
const std::string player_marker_mesh = "resources/meshes/player_marker.off";

const std::string wall_diff = "resources/textures/mossy_brick_diff_4k.jpg";
const std::string wall_norm = "resources/textures/mossy_brick_nor_gl_4k.png";
const std::string wall_rough = "resources/textures/mossy_brick_rough_4k.png";

const std::string floor_diff = "resources/textures/cobblestone_pavement_diff_4k.jpg";
const std::string floor_norm = "resources/textures/cobblestone_pavement_nor_gl_4k.png";
const std::string floor_rough = "resources/textures/cobblestone_pavement_rough_4k.png";

////////////////////
// SFML Callbacks //
////////////////////

void handle(const sf::Event::KeyPressed& key, Shaders& shaders, Minimap& minimap, Scene& scene,
            bool& running) {
    switch (key.scancode) {
        using enum sf::Keyboard::Scancode;

        case P:
            shaders.reload(phong_vert, phong_frag);
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            minimap.reloadShaders(minimap_vert, minimap_frag);
            return;
        case F:
            shaders.reload(flat_vert, flat_frag);
            shaders.use();
            scene.locations(shaders);
            scene.update_all();
            minimap.reloadShaders(minimap_vert, minimap_frag);
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
        scene.camera.processMouseMovement(dx, -dy);
        scene.lights.position(scene.camera.inv_v);
    }
}

////////////////////
// Loading Screen //
////////////////////

struct GameAssets {
    // Textures
    std::unique_ptr<Texture> wallDiff, wallNorm, wallRough;     // NOSONAR
    std::unique_ptr<Texture> floorDiff, floorNorm, floorRough;  // NOSONAR
    // Meshes
    std::unique_ptr<GPUMesh> floorMesh, wallMesh, playerMesh;  // NOSONAR
    // Materials
    std::optional<Material> wallMat, floorMat;  // NOSONAR
    // Gameplay
    std::unique_ptr<Maze> maze;
};

std::string nextLoadingStep(int step, GameAssets& assets, Minimap& minimap, Scene& scene) {
    switch (step) {
        case 0:
            assets.wallDiff = std::make_unique<Texture>(wall_diff, true);
            return "mossy bricks texture";
        case 1:
            assets.wallNorm = std::make_unique<Texture>(wall_norm);
            return "mossy bricks normals map";
        case 2:
            assets.wallRough = std::make_unique<Texture>(wall_rough);
            return "mossy bricks roughness map";
        case 3:
            assets.floorDiff = std::make_unique<Texture>(floor_diff, true);
            return "cobblestone pavement texture";
        case 4:
            assets.floorNorm = std::make_unique<Texture>(floor_norm);
            return "cobblestone pavement normals map";
        case 5:
            assets.floorRough = std::make_unique<Texture>(floor_rough);
            return "cobblestone pavement roughness map";
        case 6:
            assets.wallMesh = std::make_unique<GPUMesh>(wall_mesh);
            return "wall mesh";
        case 7:
            assets.floorMesh = std::make_unique<GPUMesh>(floor_mesh);
            return "floor mesh";
        case 8:
            assets.playerMesh = std::make_unique<GPUMesh>(player_marker_mesh);
            return "player marker mesh";
        case 9:
            assets.wallMat =
                Material{assets.wallDiff.get(), assets.wallNorm.get(), assets.wallRough.get()};
            return "wall material";
        case 10:
            assets.floorMat =
                Material{assets.floorDiff.get(), assets.floorNorm.get(), assets.floorRough.get()};
            return "floor material";
        case 11:
            assets.maze = std::make_unique<Maze>(10, 10);
            return "random maze";
        case 12: {
            Node mazeNode = assets.maze->populateSceneNode(
                assets.floorMesh.get(), assets.wallMesh.get(), *assets.wallMat, *assets.floorMat);
            scene.root.children.push_back(mazeNode);

            scene.build_static_tree();
            minimap.playerMesh = assets.playerMesh.get();
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
    Minimap minimap(minimap_vert, minimap_frag);

    shaders.use();
    Scene scene(shaders);
    GameAssets assets;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //// Main Loop ////

    const int TOTAL_STEPS = 13;
    int step = 0;

    sf::Clock deltaClock;
    bool running = true;

    while (running) {
        sf::Time dt = deltaClock.restart();

        while (const std::optional event = window.pollEvent()) {
            gui.process_event(window, *event);

            if (event->is<sf::Event::Closed>())
                running = false;
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                glViewport(0, 0, resized->size.x, resized->size.y);
                scene.camera.setAspectRatio(static_cast<float>(resized->size.x) /
                                            static_cast<float>(resized->size.y));
            } else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>();
                       key_pressed && !gui.wants_capture_keyboard()) {
                handle(*key_pressed, shaders, minimap, scene, running);
            } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>();
                       mouse && !gui.wants_capture_mouse()) {
                handle(*mouse, scene);
            }
        }

        gui.update(window, dt);

        // clear the buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (step < TOTAL_STEPS) {
            std::string what = nextLoadingStep(step, assets, minimap, scene);
            step++;

            float progress = static_cast<float>(step) / static_cast<float>(TOTAL_STEPS);
            gui.renderLoading(window, what, progress);

            window.display();
            continue;
        }

        if (!gui.wants_capture_keyboard()) {
            bool camera_moved = scene.camera.update(dt);

            if (camera_moved) {
                scene.lights.position(scene.camera.inv_v);
            }
        }

        scene.draw();

        minimap.draw(scene, gui, window);

        shaders.use();
        gui.render(scene, window);

        window.display();
    }

    window.close();

    return 0;
}
