#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <atomic>
#include <iostream>
#include <thread>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "include/assetloader.hh"
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
const std::string goal_marker_mesh = "resources/meshes/crystal.off";

const std::string wall_diff = "resources/textures/mossy_brick_diff_4k.jpg";
const std::string wall_norm = "resources/textures/mossy_brick_nor_gl_4k.png";
const std::string wall_rough = "resources/textures/mossy_brick_rough_4k.png";

const std::string floor_diff = "resources/textures/cobblestone_pavement_diff_4k.jpg";
const std::string floor_norm = "resources/textures/cobblestone_pavement_nor_gl_4k.png";
const std::string floor_rough = "resources/textures/cobblestone_pavement_rough_4k.png";

////////////
// Assets //
////////////

struct GameAssets {
    // Textures
    std::unique_ptr<Texture> wallDiff, wallNorm, wallRough;     // NOSONAR
    std::unique_ptr<Texture> floorDiff, floorNorm, floorRough;  // NOSONAR
    // Meshes
    std::unique_ptr<GPUMesh> floorMesh, wallMesh, playerMesh, goalMesh;  // NOSONAR
    // Materials
    std::optional<Material> wallMat, floorMat, goalMat;  // NOSONAR
    // Gameplay
    std::unique_ptr<Maze> maze;
};

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

///////////////////
// Asset Loading //
///////////////////

void register_asset_tasks(AssetLoader& loader, GameAssets& assets, Scene& scene, Minimap& minimap) {
    loader.addTask("mossy bricks texture",
                   [&assets]() { assets.wallDiff = std::make_unique<Texture>(wall_diff, true); });
    loader.addTask("mossy bricks normals map",
                   [&assets]() { assets.wallNorm = std::make_unique<Texture>(wall_norm); });
    loader.addTask("mossy bricks roughness map",
                   [&assets]() { assets.wallRough = std::make_unique<Texture>(wall_rough); });

    loader.addTask("cobblestone pavement texture",
                   [&assets]() { assets.floorDiff = std::make_unique<Texture>(floor_diff, true); });
    loader.addTask("cobblestone pavement normals map",
                   [&assets]() { assets.floorNorm = std::make_unique<Texture>(floor_norm); });
    loader.addTask("cobblestone pavement roughness map",
                   [&assets]() { assets.floorRough = std::make_unique<Texture>(floor_rough); });

    loader.addTask("player marker mesh", [&assets]() {
        assets.playerMesh = std::make_unique<GPUMesh>(player_marker_mesh);
    });

    loader.addTask("goal marker mesh",
                   [&assets]() { assets.goalMesh = std::make_unique<GPUMesh>(goal_marker_mesh); });

    loader.addTask("wall material", [&assets]() {
        assets.wallMat = Material{
            .diffuse = assets.wallDiff.get(),
            .normal = assets.wallNorm.get(),
            .roughness = assets.wallRough.get(),
            .diffuse_color = {0.55f, 0.55f, 0.55f},
            .ambient_color = {0.15f, 0.18f, 0.15f},
            .specular_color = {0.45f, 0.45f, 0.40f},
            .shininess = 64.0f,
        };
    });

    loader.addTask("floor material", [&assets]() {
        assets.floorMat = Material{
            .diffuse = assets.floorDiff.get(),
            .normal = assets.floorNorm.get(),
            .roughness = assets.floorRough.get(),
            .diffuse_color = {0.5f, 0.5f, 0.5f},
            .ambient_color = {0.12f, 0.12f, 0.15f},
            .specular_color = {0.15f, 0.15f, 0.15f},
            .shininess = 16.0f,
        };
    });

    loader.addTask("goal marker material", [&assets]() {
        assets.goalMat = Material{
            .diffuse_color = {0.1f, 0.9f, 0.2f},
            .ambient_color = {0.3f, 1.0f, 0.4f},
            .specular_color = {1.0f, 1.0f, 1.0f},
            .shininess = 128.0f,
            .use_textures = false,
        };
    });

    loader.addTask("random maze", [&assets]() { assets.maze = std::make_unique<Maze>(15, 15); });

    loader.addTask("scene build", [&assets, &scene, &minimap]() {
        Mesh cpuFloor(floor_mesh);
        Mesh cpuWall(wall_mesh);

        Node mazeNode =
            assets.maze->populateSceneNode(cpuFloor, cpuWall, *assets.wallMat, *assets.floorMat,
                                           assets.wallMesh, assets.floorMesh);

        scene.root.children.push_back(std::move(mazeNode));

        scene.goalNode.mesh = assets.goalMesh.get();
        scene.goalNode.material = *assets.goalMat;
        scene.goalNode.is_goal = true;

        scene.build_static_tree();

        minimap.playerMesh = assets.playerMesh.get();
    });
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //// Loading Setup ////
    AssetLoader loader;
    register_asset_tasks(loader, assets, scene, minimap);

    //// Main Loop ////
    sf::Clock deltaClock;
    bool running = true;
    bool game_initialized = false;

    auto restartGame = [&scene, &assets, &gui]() {
        scene.camera.setPosition(assets.maze->getStartWorldPosition());
        scene.camera.setYaw(135.0f);  // Face inward towards the bottom-left
        scene.camera.setPitch(0.0f);
        scene.lights.position(scene.camera.inv_v);
        gui.hasWon = false;
    };

    while (running) {
        sf::Time dt = deltaClock.restart();

        // Process Events
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

        // Clear Buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle Loading Phase
        if (!loader.isFinished()) {
            std::string what = loader.processNext();
            gui.renderLoading(window, what, loader.getProgress());
            window.display();
            continue;
        }

        if (!game_initialized) {
            restartGame();
            game_initialized = true;
        }

        // Update Game State
        if (window.hasFocus() && !gui.wants_capture_keyboard() && !gui.hasWon) {
            bool camera_moved = scene.camera.update(dt, *assets.maze.get());
            if (camera_moved) {
                scene.lights.position(scene.camera.inv_v);
            }
        }

        // Handle Animations & Win Condition
        glm::vec3 goalPos = assets.maze->getGoalWorldPosition();
        scene.update_gameplay(dt, goalPos);

        if (!gui.hasWon && scene.check_win_condition(goalPos)) {
            gui.hasWon = true;
        }

        // Render
        scene.draw();
        minimap.draw(scene, gui, window);

        shaders.use();
        gui.render(scene, window, restartGame);

        window.display();
    }

    window.close();
    return 0;
}
