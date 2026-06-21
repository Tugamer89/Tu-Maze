#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

#include <array>

#include "core/scene.hh"
#include "core/session.hh"

class Gui {
   public:
    struct GuiCallbacks {
        std::function<void(int)> onPlayRandom;
        std::function<void(unsigned int, int)> onPlayCustom;
        std::function<void()> onPlayAgain;
        std::function<void()> onReturnToMenu;
        std::function<void()> onQuitDesktop;
    };

   private:
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

    float camera_fov = 60.0f;
    float camera_sensitivity = 0.2f;
    float camera_bobbing = 0.02f;

    static constexpr std::array<const char*, 4> diffLabels = {
        "Easy (10x10)",
        "Normal (15x15)",
        "Hard (25x25)",
        "Extreme (40x40)",
    };
    static constexpr std::array<const char*, 4> diffNames = {
        "Easy",
        "Normal",
        "Hard",
        "Extreme",
    };
    static constexpr std::array<const char*, 3> qualities = {
        "High",
        "Medium",
        "Low",
    };
    static constexpr std::array<const char*, 5> msaaLabels = {
        "Off (Faster)", "2x", "4x", "8x", "16x (Max Quality)",
    };
    static constexpr std::array<int, 5> msaaValues = {0, 2, 4, 8, 16};

    void setupImGuiStyle() const;

    void loadSettings();
    void saveSettings() const;

    void renderCameraSection(Scene& scene);
    void renderVideoSection(sf::Window& window);
    void renderSettingsContent(Scene& scene, sf::Window& window);
    void renderFPSOverlay() const;
    void renderHUD(const std::string& formattedTime) const;
    void renderMinimapWindow(GLuint textureId) const;

    void renderLeaderboardTable(const SessionManager& session, int targetDiff) const;
    void renderLeaderboardSection(const SessionManager& session);
    void renderMainMenuSettingsSection(Scene& scene, sf::Window& window);
    void renderMainMenuSection(Scene& scene, sf::Window& window, const SessionManager& session,
                               const GuiCallbacks& callbacks);

    void renderVictorySection(const GuiCallbacks& callbacks) const;
    void renderPauseSection(Scene& scene, sf::Window& window, const GuiCallbacks& callbacks);

   public:
    static int getSavedMSAA();

    explicit Gui(sf::Window& window);

    ~Gui();

    Gui(const Gui&) = delete;
    Gui& operator=(Gui&&) = delete;

    [[nodiscard]] bool isMinimapEnabled() const { return minimap_enabled; }
    [[nodiscard]] bool isMinimapFixNorth() const { return minimap_fix_north; }
    [[nodiscard]] float getMinimapZoom() const { return minimap_zoom; }

    [[nodiscard]] bool hasWon() const { return won; }
    void setHasWon(bool val) { won = val; }

    [[nodiscard]] bool isPaused() const { return paused; }
    void setPaused(bool val) { paused = val; }

    [[nodiscard]] bool isInMainMenu() const { return inMainMenu; }
    void setInMainMenu(bool val) { inMainMenu = val; }

    [[nodiscard]] bool isShowingLeaderboard() const { return showLeaderboard; }
    void setShowingLeaderboard(bool val) { showLeaderboard = val; }

    [[nodiscard]] bool isShowingSettings() const { return showSettings; }
    void setShowingSettings(bool val) { showSettings = val; }

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
