#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <iostream>
#include <optional>
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
#include "include/maze.hh"
#include "include/mesh.hh"
#include "include/minimap.hh"
#include "include/node.hh"
#include "include/scene.hh"
#include "include/session.hh"
#include "include/setup.hh"

///////////////
// Constants //
///////////////

const std::string default_vert = "resources/shaders/default.vert";
const std::string default_frag = "resources/shaders/default.frag";

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
    std::unique_ptr<Texture> wallDiff;
    std::unique_ptr<Texture> wallNorm;
    std::unique_ptr<Texture> wallRough;
    std::unique_ptr<Texture> floorDiff;
    std::unique_ptr<Texture> floorNorm;
    std::unique_ptr<Texture> floorRough;

    // Base CPU Meshes
    std::unique_ptr<Mesh> baseCpuFloor;
    std::unique_ptr<Mesh> baseCpuWall;

    // Meshes
    std::unique_ptr<GPUMesh> floorMesh;
    std::unique_ptr<GPUMesh> wallMesh;
    std::unique_ptr<GPUMesh> playerMesh;
    std::unique_ptr<GPUMesh> goalMesh;

    // Materials
    std::optional<Material> wallMat;
    std::optional<Material> floorMat;
    std::optional<Material> goalMat;

    // Gameplay
    std::unique_ptr<Maze> maze;
};

////////////////////
// Mouse State    //
////////////////////

struct MouseState {
#ifdef __APPLE__
    bool ignoreNextMovement = false;
#endif
    sf::Vector2i savedMenuMousePos;
    sf::Vector2i lastMousePos;
};

// Helper function to re-center the mouse to prevent it from escaping the window
void center_mouse(const sf::Window& window, MouseState& mouseState) {
    sf::Vector2i center(window.getSize() / 2u);
    sf::Mouse::setPosition(center, window);
    mouseState.lastMousePos = sf::Mouse::getPosition(window);
#ifdef __APPLE__
    mouseState.ignoreNextMovement = true;
#endif
}

////////////////////
// SFML Callbacks //
////////////////////

void handle_key(const sf::Event::KeyPressed& key, Gui& gui, Scene& scene, bool& running) {
    if (key.scancode == sf::Keyboard::Scancode::Escape && !gui.hasWon) {
        if (!gui.inMainMenu)
            gui.isPaused = !gui.isPaused;
        else if (gui.showLeaderboard)
            gui.showLeaderboard = false;
        else if (gui.showSettings)
            gui.showSettings = false;
        else
            running = false;
    } else if (key.scancode == sf::Keyboard::Scancode::F && !gui.inMainMenu && !gui.hasWon &&
               !gui.isPaused) {
        scene.lights.is_on = !scene.lights.is_on;
        scene.update_all();
    }
}

void handle_mouse_movement(const sf::Event::MouseMoved& mouse_moved, const sf::Window& window,
                           const Gui& gui, Scene& scene, MouseState& mouseState) {
    // Only process camera movement when we are actively playing
    if (!window.hasFocus() || gui.isPaused || gui.hasWon || gui.inMainMenu) {
        return;
    }

#ifdef __APPLE__
    // Catch the intentional event triggered by center_mouse
    if (mouseState.ignoreNextMovement) {
        mouseState.ignoreNextMovement = false;

        if (mouseState.lastMousePos == mouse_moved.position) return;
    }
#endif

    auto dx = static_cast<float>(mouse_moved.position.x - mouseState.lastMousePos.x);
    auto dy = static_cast<float>(mouse_moved.position.y - mouseState.lastMousePos.y);

    if (dx != 0 || dy != 0) {  // NOSONAR
        scene.camera.processMouseMovement(dx, -dy);
        scene.lights.position(scene.camera.inv_v);
        center_mouse(window, mouseState);
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

    loader.addTask("world meshes", [&assets]() {
        assets.baseCpuFloor = std::make_unique<Mesh>(floor_mesh);
        assets.baseCpuWall = std::make_unique<Mesh>(wall_mesh);
        assets.playerMesh = std::make_unique<GPUMesh>(player_marker_mesh);
        assets.goalMesh = std::make_unique<GPUMesh>(goal_marker_mesh);
    });

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
            .alpha = 0.4f,
            .use_textures = false,
            .use_flat = true,
        };
    });

    loader.addTask("scene build", [&assets, &scene, &minimap]() {
        scene.goalNode.mesh = assets.goalMesh.get();
        scene.goalNode.material = &assets.goalMat.value();
        scene.goalNode.is_goal = true;
        scene.goalNode.updateTransforms();

        minimap.playerMesh = assets.playerMesh.get();
    });
}

//////////////////////////////////
// Game State & Event Functions //
//////////////////////////////////

void process_events(sf::Window& window, Gui& gui, Scene& scene, bool& running,
                    MouseState& mouseState) {
    while (const std::optional event = window.pollEvent()) {
        gui.process_event(window, *event);

        if (event->is<sf::Event::Closed>())
            running = false;
        else if (event->is<sf::Event::FocusLost>() && !gui.hasWon && !gui.inMainMenu)
            gui.isPaused = true;
        else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            glViewport(0, 0, resized->size.x, resized->size.y);
            scene.camera.setAspectRatio(static_cast<float>(resized->size.x) /
                                        static_cast<float>(resized->size.y));
        } else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>();
                   key_pressed && !gui.wants_capture_keyboard()) {
            handle_key(*key_pressed, gui, scene, running);
        } else if (const auto* mouse_moved = event->getIf<sf::Event::MouseMoved>()) {
            handle_mouse_movement(*mouse_moved, window, gui, scene, mouseState);
        }
    }
}

// Strictly handles transitions between Game/Pause/Menu logic (cursor visibilities)
bool update_pause_state(sf::Window& window, const Gui& gui, SessionManager& session,
                        bool& wasPaused, MouseState& mouseState) {
    bool isGameActive = window.hasFocus() && !gui.isPaused && !gui.hasWon && !gui.inMainMenu;

    if (wasPaused && isGameActive) {
        // Menu/Pause -> In-Game transition
        mouseState.savedMenuMousePos = sf::Mouse::getPosition(window);

        session.start();

        window.setMouseCursorVisible(false);
        window.setMouseCursorGrabbed(true);

        center_mouse(window, mouseState);
    } else if (!wasPaused && !isGameActive) {
        // In-Game -> Menu/Pause transition
        session.stop();

        window.setMouseCursorGrabbed(false);
        window.setMouseCursorVisible(true);

        // Restore mouse to where it was left off in the menu
        sf::Mouse::setPosition(mouseState.savedMenuMousePos, window);
    }

    wasPaused = !isGameActive;
    return isGameActive;
}

//////////
// Main //
//////////

int main(int argc, char* argv[]) {
    //// Startup ////
    Setup setup;
    sf::Window& window = setup.window;

    Gui gui(window);

    Shaders shaders(default_vert, default_frag);
    Minimap minimap(minimap_vert, minimap_frag);

    shaders.use();
    Scene scene(shaders);
    GameAssets assets;
    SessionManager session;
    MouseState mouseState;

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
    bool wasPaused = false;

    // State to track how the game was launched so "Play Again" can reuse it
    std::optional<unsigned int> currentCustomSeed = std::nullopt;
    int currentDifficulty = 1;

    // Levels: [Easy, Normal, Hard, Extreme]
    const std::array<int, 4> difficultySizes{10, 15, 25, 40};

    auto resetMaze = [&scene, &assets, &difficultySizes](
                         std::optional<unsigned int> seed = std::nullopt, int diffIdx = 1) {
        scene.root.children.clear();

        int size = difficultySizes[diffIdx];
        assets.maze = std::make_unique<Maze>(size, size, seed);
        Node mazeNode = assets.maze->populateSceneNode(*assets.baseCpuFloor, *assets.baseCpuWall,
                                                       *assets.wallMat, *assets.floorMat,
                                                       assets.wallMesh, assets.floorMesh);

        scene.root.children.push_back(std::move(mazeNode));
        scene.build_static_tree();

        scene.camera.setPosition(assets.maze->getStartWorldPosition());
        scene.camera.setYaw(135.0f);  // Face inward towards the bottom-left
        scene.camera.setPitch(0.0f);
        scene.lights.is_on = true;
        scene.lights.position(scene.camera.inv_v);

        scene.update_all();
    };

    auto restartGame = [&gui, &session, &window, &currentDifficulty, &mouseState, resetMaze](
                           std::optional<unsigned int> seed, int diffIdx) {
        currentDifficulty = diffIdx;
        resetMaze(seed, diffIdx);

        gui.hasWon = false;
        gui.isPaused = false;
        gui.inMainMenu = false;

        center_mouse(window, mouseState);

        session.reset();
        session.start();
    };

    Gui::GuiCallbacks callbacks{
        .onPlayRandom =
            [&currentCustomSeed, restartGame](int diffIdx) {
                currentCustomSeed = std::nullopt;
                restartGame(std::nullopt, diffIdx);
            },
        .onPlayCustom =
            [&currentCustomSeed, restartGame](unsigned int seed, int diffIdx) {
                currentCustomSeed = seed;
                restartGame(seed, diffIdx);
            },
        .onPlayAgain = [&currentCustomSeed, &currentDifficulty,
                        restartGame]() { restartGame(currentCustomSeed, currentDifficulty); },
        .onReturnToMenu =
            [&gui, &session]() {
                gui.inMainMenu = true;
                gui.hasWon = false;
                gui.isPaused = false;
                session.stop();
            },
        .onQuitDesktop = [&running]() { running = false; },
    };

    while (running) {
        sf::Time dt = deltaClock.restart();

        // Process Events
        process_events(window, gui, scene, running, mouseState);

        gui.update(window, dt);

        // Clear Buffers
        if (gui.inMainMenu)
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        else
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handle Loading Phase
        if (!loader.isFinished()) {
            std::string what = loader.processNext();
            gui.renderLoading(window, what, loader.getProgress());
            window.display();
            mouseState.savedMenuMousePos = sf::Mouse::getPosition(window);
            continue;
        }

        // Handle Status Transitions
        if (update_pause_state(window, gui, session, wasPaused, mouseState)) {
            session.update();

            // Safe assumption because Mouse Grab implies the game is actively running
            if (assets.maze && scene.camera.update(dt, *assets.maze.get())) {
                scene.lights.position(scene.camera.inv_v);
            }
        }

        // Render Background/Game & Handle Rules only when we are out of the main menu
        if (!gui.inMainMenu && assets.maze) {
            glm::vec3 goalPos = assets.maze->getGoalWorldPosition();
            scene.update_gameplay(dt, goalPos);

            if (!gui.hasWon && scene.check_win_condition(goalPos)) {
                gui.hasWon = true;
                gui.isPaused = false;
                session.stop();
                session.saveScore(assets.maze->currentSeed, currentDifficulty);
            }

            scene.draw();
            minimap.draw(scene, gui);
        }

        shaders.use();
        GLuint minimapTex = assets.maze ? minimap.getTextureID() : 0;
        gui.renderUI(scene, window, session, callbacks, minimapTex);

        window.display();
    }

    window.close();
    return 0;
}
