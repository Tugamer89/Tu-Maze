#ifndef GUI_HH
#define GUI_HH

#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <concepts>
#include <fstream>
#include <iostream>
#include <string>

#include "scene.hh"
#include "texture.hh"

class Gui {
   public:
    inline static const std::string settingsFile = "tu-maze_settings.txt";

    bool minimap_enabled = true;
    bool minimap_fix_north = true;
    float minimap_zoom = 8.0f;
    bool hasWon = false;
    bool isPaused = false;
    bool show_fps_overlay = false;

   private:
    bool vsync_enabled = true;
    int msaa_level = 0;
    int active_msaa_level = 0;
    float camera_fov = 60.0f;

    void setupImGuiStyle() const {
        ImGuiStyle& style = ImGui::GetStyle();

        // Rounding
        style.WindowRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.TabRounding = 6.0f;

        // Padding
        style.WindowPadding = ImVec2(16.0f, 16.0f);
        style.FramePadding = ImVec2(12.0f, 8.0f);
        style.ItemSpacing = ImVec2(8.0f, 12.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);

        // Palette Colori (Modern Dark Theme con accenti blu)
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.98f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.80f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    }

    void loadSettings() {
        std::ifstream file(settingsFile);
        if (file.is_open()) {
            if (int q; file >> q) {
                q = std::clamp(q, 0, 2);
                Texture::currentGlobalQuality = static_cast<TextureQuality>(q);
            }

            file >> vsync_enabled;
            file >> msaa_level;
            file >> camera_fov;
            file >> minimap_enabled;
            file >> minimap_fix_north;
            file >> minimap_zoom;
            file >> show_fps_overlay;

            file.close();
        } else {
            Texture::currentGlobalQuality = Texture::autoDetectQuality();
            saveSettings();
        }
    }

    void saveSettings() const {
        std::ofstream file(settingsFile);
        if (file.is_open()) {
            file << static_cast<int>(Texture::currentGlobalQuality) << "\n";
            file << vsync_enabled << "\n";
            file << msaa_level << "\n";
            file << camera_fov << "\n";
            file << minimap_enabled << "\n";
            file << minimap_fix_north << "\n";
            file << minimap_zoom << "\n";
            file << show_fps_overlay << "\n";
            file.close();
        }
    }

    void renderCameraSection(Scene& scene) {
        // FOV
        if (ImGui::SliderFloat("Field of View", &camera_fov, 30.0f, 120.0f, "%.1f deg")) {
            scene.camera.setFov(camera_fov);
            saveSettings();
        }

        ImGui::Separator();
        ImGui::Text("Minimap Settings");

        if (ImGui::Checkbox("Show Minimap", &minimap_enabled)) saveSettings();

        if (minimap_enabled) {
            if (ImGui::Checkbox("Fix North", &minimap_fix_north)) saveSettings();
            if (ImGui::SliderFloat("Zoom", &minimap_zoom, 3.0f, 30.0f, "%.1f units"))
                saveSettings();
        }
    }

    void renderVideoSection(sf::Window& window) {
        if (ImGui::Checkbox("Show FPS Overlay", &show_fps_overlay)) {
            saveSettings();
        }

        ImGui::Separator();

        // Texture Quality
        auto currentQuality = static_cast<int>(Texture::currentGlobalQuality);
        if (std::array<const char*, 3> qualities = {"High", "Medium", "Low"};
            ImGui::Combo("Texture Quality", &currentQuality, qualities.data(),
                         static_cast<int>(qualities.size()))) {
            Texture::setGlobalQuality(static_cast<TextureQuality>(currentQuality));
            saveSettings();
        }

        // V-Sync
        if (ImGui::Checkbox("V-Sync", &vsync_enabled)) {
            window.setVerticalSyncEnabled(vsync_enabled);
            saveSettings();
        }

        // Anti-Aliasing
        std::array<int, 5> msaa_values = {0, 2, 4, 8, 16};
        std::array<const char*, 5> msaa_labels = {"Off (Faster)", "2x", "4x", "8x",
                                                  "16x (Max Quality)"};

        int current_msaa_idx = 0;
        for (size_t i = 0; i < msaa_values.size(); ++i) {
            if (msaa_level == msaa_values[i]) current_msaa_idx = static_cast<int>(i);
        }

        if (ImGui::Combo("Anti-Aliasing (MSAA)", &current_msaa_idx, msaa_labels.data(),
                         static_cast<int>(msaa_labels.size()))) {
            msaa_level = msaa_values[current_msaa_idx];
            saveSettings();
        }

        if (msaa_level != active_msaa_level) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                               "Restart required to apply MSAA changes.");
        }
    }

    void renderFPSOverlay() const {
        if (!show_fps_overlay) return;

        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoBackground;

        ImGui::SetNextWindowPos(ImVec2(15.0f, 15.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.0f);

        if (ImGui::Begin("FPSOverlay", nullptr, windowFlags)) {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "FPS: %.0f",
                               ImGui::GetIO().Framerate);
        }
        ImGui::End();
    }

   public:
    static int getSavedMSAA() {
        if (std::ifstream file(settingsFile); file.is_open()) {
            int q;
            int m;
            bool v;
            // Read until MSAA
            if (file >> q >> v >> m) {
                return m;
            }
        }
        return 0;
    }

    explicit Gui(sf::Window& window) {
        if (!ImGui::SFML::Init(window, sf::Vector2f(window.getSize()), false)) {
            std::cerr << "Error during ImGui-SFML initialization!" << std::endl;
            exit(1);
        }
        // Start the native OpenGL 3 backend for GUI rendering
        ImGui_ImplOpenGL3_Init("#version 410 core");

        // Apply modern theme
        setupImGuiStyle();

        active_msaa_level = getSavedMSAA();
        loadSettings();

        window.setVerticalSyncEnabled(vsync_enabled);

        if (active_msaa_level > 0)
            glEnable(GL_MULTISAMPLE);
        else
            glDisable(GL_MULTISAMPLE);
    }

    ~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::SFML::Shutdown();
    }

    Gui(const Gui&) = delete;
    Gui& operator=(Gui&&) = delete;

    // Forward SFML events to ImGui
    void process_event(const sf::Window& window, const sf::Event& event) const {
        ImGui::SFML::ProcessEvent(window, event);
    }

    // Indicates if ImGui is currently capturing the keyboard
    bool wants_capture_keyboard() const { return ImGui::GetIO().WantCaptureKeyboard; }

    // Indicates if ImGui is currently capturing the mouse
    bool wants_capture_mouse() const { return ImGui::GetIO().WantCaptureMouse; }

    // Prepares a new frame for the GUI
    void update(const sf::Window& window, sf::Time dt) const {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::SFML::Update(sf::Mouse::getPosition(window), sf::Vector2f(window.getSize()), dt);
    }

    void renderSettings(Scene& scene, sf::Window& window, std::invocable auto onRestart,
                        std::invocable auto onQuit) {
        // Overlay Victory Modal
        if (hasWon) {
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::Begin("Victory!", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoMove);

            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Congratulations!");
            ImGui::Text("You successfully navigated out of the maze.");
            ImGui::Separator();

            float buttonWidth = 120.0f;

            if (ImGui::Button("Exit", ImVec2(buttonWidth, 0))) {
                onQuit();
            }

            float rightAlignX =
                ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().WindowPadding.x;

            ImGui::SameLine(rightAlignX);

            if (ImGui::Button("Play Again", ImVec2(buttonWidth, 0))) {
                hasWon = false;
                onRestart();
            }

            ImGui::End();
        }

        // Pause Menu
        if (isPaused) {
            ImGui::SetNextWindowPos(
                ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                ImGuiCond_Always, ImVec2(0.5f, 0.5f));

            // Fixed Size
            ImGui::SetNextWindowSize(ImVec2(520.0f, 460.0f), ImGuiCond_Appearing);

            // No resize and no move
            ImGuiWindowFlags pauseFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                          ImGuiWindowFlags_NoMove |
                                          ImGuiWindowFlags_NoSavedSettings;

            ImGui::Begin("Game Paused", &isPaused, pauseFlags);

            if (ImGui::Button("Resume Game", ImVec2(-1.0f, 38.0f))) {
                isPaused = false;
            }
            if (ImGui::Button("Quit to Desktop", ImVec2(-1.0f, 38.0f))) {
                onQuit();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::BeginTabBar("SettingsTabs")) {
                if (ImGui::BeginTabItem("Video & Display")) {
                    ImGui::Spacing();
                    renderVideoSection(window);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Gameplay & UI")) {
                    ImGui::Spacing();
                    renderCameraSection(scene);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::End();
        }

        if (static bool isFirstFrame = true; isFirstFrame) {
            scene.camera.setFov(camera_fov);
            isFirstFrame = false;
        }

        renderFPSOverlay();

        // Generate draw data
        ImGui::Render();

        // Disable MSAA
        glDisable(GL_MULTISAMPLE);

        // Draw ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Enable MSAA if necessary
        if (active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
    }

    void renderLoading(const sf::Window& window, const std::string& what, float progress) const {
        ImVec2 windowSize(static_cast<float>(window.getSize().x),
                          static_cast<float>(window.getSize().y));

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(windowSize);

        // Push window styles for a borderless and dark background
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        // Disable inputs and saving settings for this window
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;

        ImGui::Begin("Loading", nullptr, flags);

        // Prepare text and calculate dimensions for centering
        std::string text = "Loading " + what + "...";
        ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

        float barWidth = windowSize.x * 0.5f;  // Bar takes up 50% of screen width
        float barHeight = 24.0f;
        float spacing = 12.0f;

        // Calculate starting Y position to vertically center the entire block
        float totalContentHeight = textSize.y + spacing + barHeight;
        float startY = (windowSize.y - totalContentHeight) * 0.5f;

        // Render Centered Text
        ImGui::SetCursorPosY(startY);
        ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
        ImGui::Text("%s", text.c_str());

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.70f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
        ImGui::SetCursorPosX((windowSize.x - barWidth) * 0.5f);
        ImGui::ProgressBar(progress, ImVec2(barWidth, barHeight));

        // Restore progress bar styles
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        ImGui::End();

        // Restore window styles
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::Render();

        // Disable MSAA
        glDisable(GL_MULTISAMPLE);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Enable MSAA if necessary
        if (active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
    }

    void renderHUD(const std::string& formattedTime) const {
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoInputs;

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImVec2 windowPos(displaySize.x * 0.5f, 30.0f);
        ImVec2 windowPosPivot(0.5f, 0.0f);

        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPosPivot);
        ImGui::SetNextWindowBgAlpha(0.0f);

        if (ImGui::Begin("GameHUD", nullptr, windowFlags)) {
            ImGui::SetWindowFontScale(2.5f);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", formattedTime.c_str());
            ImGui::SetWindowFontScale(1.0f);
        }
        ImGui::End();
    }
};

#endif
