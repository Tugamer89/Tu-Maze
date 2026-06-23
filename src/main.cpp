#include <SFML/System/Clock.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <stdexcept>

#include "core/camera.hpp"
#include "core/exceptions.hpp"
#include "core/node.hpp"
#include "core/rawmouse.hpp"
#include "core/scene.hpp"
#include "core/session.hpp"
#include "core/setup.hpp"
#include "game/maze.hpp"
#include "game/minimap.hpp"
#include "glad/gl.h"
#include "render/gpumesh.hpp"
#include "render/hotshaders.hpp"
#include "render/lights.hpp"
#include "render/mesh.hpp"
#include "ui/gui.hpp"
#include "utils/assetloader.hpp"

// Constants
constexpr const char* default_vert = "resources/shaders/default.vert";
constexpr const char* default_frag = "resources/shaders/default.frag";
constexpr const char* minimap_vert = "resources/shaders/minimap.vert";
constexpr const char* minimap_frag = "resources/shaders/minimap.frag";

constexpr const char* floor_mesh = "resources/meshes/floor.off";
constexpr const char* wall_mesh = "resources/meshes/wall.off";
constexpr const char* player_marker_mesh = "resources/meshes/player_marker.off";
constexpr const char* goal_marker_mesh = "resources/meshes/crystal.off";

constexpr const char* wall_diff = "resources/textures/mossy_brick_diff_4k.jpg";
constexpr const char* wall_norm = "resources/textures/mossy_brick_nor_gl_4k.png";
constexpr const char* wall_rough = "resources/textures/mossy_brick_rough_4k.png";

constexpr const char* floor_diff = "resources/textures/cobblestone_pavement_diff_4k.jpg";
constexpr const char* floor_norm = "resources/textures/cobblestone_pavement_nor_gl_4k.png";
constexpr const char* floor_rough = "resources/textures/cobblestone_pavement_rough_4k.png";

// Game Assets Context holding all memory allocations
struct GameAssets {
    std::unique_ptr<Texture> wallDiff;
    std::unique_ptr<Texture> wallNorm;
    std::unique_ptr<Texture> wallRough;
    std::unique_ptr<Texture> floorDiff;
    std::unique_ptr<Texture> floorNorm;
    std::unique_ptr<Texture> floorRough;

    std::unique_ptr<Mesh> baseCpuFloor;
    std::unique_ptr<Mesh> baseCpuWall;

    std::unique_ptr<GPUMesh> floorMesh;
    std::unique_ptr<GPUMesh> wallMesh;
    std::unique_ptr<GPUMesh> playerMesh;
    std::unique_ptr<GPUMesh> goalMesh;

    std::optional<Material> wallMat;
    std::optional<Material> floorMat;
    std::optional<Material> goalMat;

    std::unique_ptr<Maze> maze;
};

// Mouse State tracking for UI interaction recovery
struct MouseState {
    sf::Vector2i savedMenuMousePos;
};

// SFML Event Callbacks
void handle_key(const sf::Event::KeyPressed& key, Gui& gui, Scene& scene, bool& running) {
    if (key.scancode == sf::Keyboard::Scancode::Escape && !gui.hasWon()) {
        if (!gui.isInMainMenu())
            gui.setPaused(!gui.isPaused());
        else if (gui.isShowingLeaderboard())
            gui.setShowingLeaderboard(false);
        else if (gui.isShowingSettings())
            gui.setShowingSettings(false);
        else
            running = false;
    } else if (key.scancode == sf::Keyboard::Scancode::F && !gui.isInMainMenu() && !gui.hasWon() &&
               !gui.isPaused()) {
        // Toggle global flashlight/directional light
        scene.getLights().setOn(!scene.getLights().isOn());
        scene.update_all();
    }
}

// Asynchronous Asset Initialization Pipeline
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
        scene.getGoalNode().setMesh(assets.goalMesh.get());
        scene.getGoalNode().setMaterial(&assets.goalMat.value());
        scene.getGoalNode().setIsGoal(true);
        scene.getGoalNode().updateTransforms();

        minimap.setPlayerMesh(assets.playerMesh.get());
    });
}

void process_events(sf::Window& window, Gui& gui, Scene& scene, bool& running, RawMouse& rawMouse) {
    while (const std::optional event = window.pollEvent()) {
        gui.process_event(window, *event);

        if (event->is<sf::Event::Closed>())
            running = false;
        else if (event->is<sf::Event::FocusLost>() && !gui.hasWon() && !gui.isInMainMenu())
            gui.setPaused(true);
        else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            glViewport(0, 0, resized->size.x, resized->size.y);
            scene.getCamera().setAspectRatio(static_cast<float>(resized->size.x) /
                                             static_cast<float>(resized->size.y));
        } else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>();
                   key_pressed && !gui.wants_capture_keyboard()) {
            handle_key(*key_pressed, gui, scene, running);
        } else if (const auto* mouse_raw = event->getIf<sf::Event::MouseMovedRaw>()) {
            rawMouse.event(*mouse_raw);
        }
    }
}

// Transitions between Game/Pause/Menu logic (toggles cursor visibilities and game timers)
bool update_pause_state(sf::Window& window, const Gui& gui, SessionManager& session,
                        bool& wasPaused, MouseState& mouseState, RawMouse& rawMouse) {
    bool isGameActive =
        window.hasFocus() && !gui.isPaused() && !gui.hasWon() && !gui.isInMainMenu();

    if (wasPaused && isGameActive) {
        // Returning to game: hide cursor and lock it to the window
        mouseState.savedMenuMousePos = sf::Mouse::getPosition();
        session.start();
        window.setMouseCursorVisible(false);
        window.setMouseCursorGrabbed(true);
        rawMouse.delta();  // Discard any accumulated mouse movement
    } else if (!wasPaused && !isGameActive) {
        // Pausing game: show cursor and free it for UI interaction
        session.stop();
        window.setMouseCursorGrabbed(false);
        window.setMouseCursorVisible(true);
        rawMouse.setPosition(mouseState.savedMenuMousePos);
    }

    wasPaused = !isGameActive;
    return isGameActive;
}

int run_engine() {
    Setup setup;
    sf::Window& window = setup.getWindow();
    Gui gui(window);

    Shaders shaders(default_vert, default_frag);
    Minimap minimap(minimap_vert, minimap_frag);

    shaders.use();
    Scene scene(shaders);
    GameAssets assets;
    SessionManager session;
    MouseState mouseState;
    RawMouse rawMouse;

    AssetLoader loader;
    register_asset_tasks(loader, assets, scene, minimap);

    sf::Clock deltaClock;
    bool running = true;
    bool wasPaused = false;

    std::optional<unsigned int> currentCustomSeed = std::nullopt;
    int currentDifficulty = 1;
    const std::array<int, 4> difficultySizes{10, 15, 25, 40};

    // Rebuilds the maze dynamically without requiring a full engine restart
    auto resetMaze = [&scene, &assets, &difficultySizes](
                         std::optional<unsigned int> seed = std::nullopt, int diffIdx = 1) {
        scene.getRoot().clearChildren();

        int size = difficultySizes[diffIdx];
        assets.maze = std::make_unique<Maze>(size, size, seed);
        Node mazeNode = assets.maze->populateSceneNode(*assets.baseCpuFloor, *assets.baseCpuWall,
                                                       *assets.wallMat, *assets.floorMat,
                                                       assets.wallMesh, assets.floorMesh);

        scene.getRoot().addChild(std::move(mazeNode));
        scene.build_static_tree();

        // Reset camera to the new maze's spawn location
        scene.getCamera().setPosition(assets.maze->getStartWorldPosition());
        scene.getCamera().setYaw(135.0f);
        scene.getCamera().setPitch(0.0f);
        scene.getLights().setOn(true);
        scene.getLights().position(scene.getCamera().getInverseViewMatrix());

        scene.update_all();
    };

    auto restartGame = [&gui, &session, &currentDifficulty, &rawMouse, resetMaze](
                           std::optional<unsigned int> seed, int diffIdx) {
        currentDifficulty = diffIdx;
        resetMaze(seed, diffIdx);

        gui.setHasWon(false);
        gui.setPaused(false);
        gui.setInMainMenu(false);

        rawMouse.delta();  // Discard initial jumps before the first frame
        session.reset();
        session.start();
    };

    // Bind GUI events to core engine logic
    GuiCallbacks callbacks{
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
                gui.setInMainMenu(true);
                gui.setHasWon(false);
                gui.setPaused(false);
                session.stop();
            },
        .onQuitDesktop = [&running]() { running = false; },
    };

    // Main Engine Loop
    while (running) {
        sf::Time dt = deltaClock.restart();

        process_events(window, gui, scene, running, rawMouse);
        gui.update(window, dt);

        if (gui.isInMainMenu())
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        else
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render Loading Screen if assets are still unpacking
        if (!loader.isFinished()) {
            std::string what = loader.processNext();
            gui.renderLoading(window, what, loader.getProgress());
            window.display();
            mouseState.savedMenuMousePos = sf::Mouse::getPosition(window);
            continue;
        }

        // Process physics and camera movement only if actively playing
        if (update_pause_state(window, gui, session, wasPaused, mouseState, rawMouse)) {
            session.update();

            if (sf::Vector2f mDelta = rawMouse.delta();
                std::abs(mDelta.x) > 0.0f || std::abs(mDelta.y) > 0.0f) {
                scene.getCamera().processMouseMovement(mDelta.x, -mDelta.y);
                scene.getLights().position(scene.getCamera().getInverseViewMatrix());
            }

            if (scene.getCamera().update(dt, *assets.maze.get())) {
                scene.getLights().position(scene.getCamera().getInverseViewMatrix());
            }
        }

        if (!gui.isInMainMenu()) {
            glm::vec3 goalPos = assets.maze->getGoalWorldPosition();
            scene.update_gameplay(dt, goalPos);

            if (!gui.hasWon() && scene.check_win_condition(goalPos)) {
                gui.setHasWon(true);
                gui.setPaused(false);
                session.stop();
                session.saveScore(assets.maze->getCurrentSeed(), currentDifficulty);
            }

            scene.draw();
            minimap.draw(scene, gui);
        }

        // Render ImGui Overlays
        shaders.use();
        gui.renderUI(scene, window, session, callbacks, minimap.getTextureID());

        window.display();
    }

    window.close();
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    try {
        return run_engine();
    } catch (const exceptions::EngineSetupException& e) {
        std::cerr << "Engine Setup Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const exceptions::ShaderException& e) {
        std::cerr << "Shader Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const exceptions::MeshException& e) {
        std::cerr << "Mesh Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
