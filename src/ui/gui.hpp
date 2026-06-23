#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Window.hpp>
#include <functional>
#include <string>

#include "core/scene.hpp"
#include "core/session.hpp"

struct GuiCallbacks {
    std::function<void(int)> onPlayRandom;
    std::function<void(unsigned int, int)> onPlayCustom;
    std::function<void()> onPlayAgain;
    std::function<void()> onReturnToMenu;
    std::function<void()> onQuitDesktop;
};

// State Class
struct GuiState {
    static constexpr const char* settingsFile = "tu-maze_settings.txt";

    bool minimap_enabled = true;
    bool minimap_fix_north = true;
    float minimap_zoom = 8.0f;
    bool won = false;
    bool paused = false;
    bool show_fps_overlay = false;

    bool inMainMenu = true;
    bool showLeaderboard = false;
    bool showSettings = false;
    int customSeedInput = 0;
    int selectedDifficulty = 1;

    bool vsync_enabled = true;
    int msaa_level = 0;
    int active_msaa_level = 0;

    bool fullscreen = false;
    bool active_fullscreen = false;

    float camera_fov = 60.0f;
    float camera_sensitivity = 0.2f;
    float camera_bobbing = 0.02f;

    void loadSettings();
    void saveSettings() const;
    static int getSavedMSAA();
    static bool getSavedFullscreen();
};

// UI Class for Menus
class GuiMenus {
   public:
    static void renderMainMenu(GuiState& state, Scene& scene, sf::Window& window,
                               const SessionManager& session, const GuiCallbacks& callbacks);
    static void renderPauseMenu(GuiState& state, Scene& scene, sf::Window& window,
                                const GuiCallbacks& callbacks);
    static void renderVictory(const GuiCallbacks& callbacks);
    static void renderSettingsContent(GuiState& state, Scene& scene, sf::Window& window);

   private:
    static void renderControlsSection(GuiState& state, Scene& scene);
    static void renderMinimapSection(GuiState& state);
    static void renderVideoSection(GuiState& state, sf::Window& window);
    static void renderLeaderboardSection(GuiState& state, const SessionManager& session);
    static void renderLeaderboardTable(const SessionManager& session, int targetDiff);
};

// UI Class for Overlays/HUD
class GuiOverlays {
   public:
    static void renderFPS(bool show);
    static void renderHUD(const std::string& formattedTime);
    static void renderMinimap(bool enabled, GLuint textureId);
    static void renderLoading(const sf::Window& window, const std::string& what, float progress);
};

// Main Facade Class
class Gui {
   private:
    GuiState state;

    void setupImGuiStyle() const;

   public:
    explicit Gui(sf::Window& window);
    ~Gui();

    Gui(const Gui&) = delete;
    Gui& operator=(Gui&&) = delete;

    [[nodiscard]] bool isMinimapEnabled() const { return state.minimap_enabled; }
    [[nodiscard]] bool isMinimapFixNorth() const { return state.minimap_fix_north; }
    [[nodiscard]] float getMinimapZoom() const { return state.minimap_zoom; }

    [[nodiscard]] bool hasWon() const { return state.won; }
    void setHasWon(bool val) { state.won = val; }

    [[nodiscard]] bool isPaused() const { return state.paused; }
    void setPaused(bool val) { state.paused = val; }

    [[nodiscard]] bool isInMainMenu() const { return state.inMainMenu; }
    void setInMainMenu(bool val) { state.inMainMenu = val; }

    [[nodiscard]] bool isShowingLeaderboard() const { return state.showLeaderboard; }
    void setShowingLeaderboard(bool val) { state.showLeaderboard = val; }

    [[nodiscard]] bool isShowingSettings() const { return state.showSettings; }
    void setShowingSettings(bool val) { state.showSettings = val; }

    void process_event(const sf::Window& window, const sf::Event& event) const {
        ImGui::SFML::ProcessEvent(window, event);
    }

    [[nodiscard]] bool wants_capture_keyboard() const { return ImGui::GetIO().WantCaptureKeyboard; }
    [[nodiscard]] bool wants_capture_mouse() const { return ImGui::GetIO().WantCaptureMouse; }

    void update(const sf::Window& window, sf::Time dt) const;

    // Main draw loop for all 2D Overlay interfaces (Minimap, HUD, Main Menu)
    void renderUI(Scene& scene, sf::Window& window, const SessionManager& session,
                  const GuiCallbacks& callbacks, GLuint minimapTexture);

    void renderLoading(const sf::Window& window, const std::string& what, float progress) const;
};
