#include "ui/gui.hh"

#include <imgui_impl_opengl3.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <concepts>
#include <ctime>
#include <fstream>
#include <functional>

#include "core/exceptions.hh"

void Gui::setupImGuiStyle() const {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;

    style.WindowPadding = ImVec2(16.0f, 16.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(8.0f, 12.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);

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

void Gui::loadSettings() {
    std::ifstream file(settingsFile);
    if (file.is_open()) {
        int q = 0;
        file >> q;
        q = std::clamp(q, 0, 2);
        Texture::setGlobalQuality(static_cast<TextureQuality>(q));

        file >> vsync_enabled >> msaa_level >> camera_fov >> minimap_enabled >> minimap_fix_north >>
            minimap_zoom >> show_fps_overlay >> selectedDifficulty >> camera_sensitivity >>
            camera_bobbing;

        selectedDifficulty = std::clamp(selectedDifficulty, 0, 3);
        file.close();
    } else {
        Texture::setGlobalQuality(Texture::autoDetectQuality());
        saveSettings();
    }
}

void Gui::saveSettings() const {
    std::ofstream file(settingsFile);
    if (file.is_open()) {
        file << static_cast<int>(Texture::getGlobalQuality()) << "\n"
             << vsync_enabled << "\n"
             << msaa_level << "\n"
             << camera_fov << "\n"
             << minimap_enabled << "\n"
             << minimap_fix_north << "\n"
             << minimap_zoom << "\n"
             << show_fps_overlay << "\n"
             << selectedDifficulty << "\n"
             << camera_sensitivity << "\n"
             << camera_bobbing << "\n";
        file.close();
    }
}

void Gui::renderCameraSection(Scene& scene) {
    ImGui::TextColored(ImVec4(0.2f, 0.70f, 1.0f, 1.0f), "Controls & Camera");

    if (ImGui::SliderFloat("Mouse Sensitivity", &camera_sensitivity, 0.01f, 1.0f, "%.2f")) {
        scene.getCamera().setMouseSensitivity(camera_sensitivity);
        saveSettings();
    }

    if (ImGui::SliderFloat("Head Bobbing", &camera_bobbing, 0.0f, 0.05f, "%.3f")) {
        scene.getCamera().setBobbingAmount(camera_bobbing);
        saveSettings();
    }

    if (ImGui::SliderFloat("Field of View", &camera_fov, 30.0f, 120.0f, "%.1f deg")) {
        scene.getCamera().setFov(camera_fov);
        saveSettings();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(0.2f, 0.70f, 1.0f, 1.0f), "Minimap Settings");

    if (ImGui::Checkbox("Show Minimap", &minimap_enabled)) saveSettings();

    if (minimap_enabled) {
        if (ImGui::Checkbox("Fix North", &minimap_fix_north)) saveSettings();
        if (ImGui::SliderFloat("Zoom", &minimap_zoom, 3.0f, 30.0f, "%.1f units")) saveSettings();
    }
}

void Gui::renderVideoSection(sf::Window& window) {
    if (ImGui::Checkbox("Show FPS Overlay", &show_fps_overlay)) {
        saveSettings();
    }

    ImGui::Separator();

    if (auto currentQuality = static_cast<int>(Texture::getGlobalQuality());
        ImGui::Combo("Texture Quality", &currentQuality, qualities.data(),
                     static_cast<int>(qualities.size()))) {
        Texture::setGlobalQuality(static_cast<TextureQuality>(currentQuality));
        saveSettings();
    }

    if (ImGui::Checkbox("V-Sync", &vsync_enabled)) {
        window.setVerticalSyncEnabled(vsync_enabled);
        saveSettings();
    }

    int current_msaa_idx = 0;
    for (size_t i = 0; i < msaaValues.size(); ++i) {
        if (msaa_level == msaaValues[i]) current_msaa_idx = static_cast<int>(i);
    }

    if (ImGui::Combo("Anti-Aliasing (MSAA)", &current_msaa_idx, msaaLabels.data(),
                     static_cast<int>(msaaLabels.size()))) {
        msaa_level = msaaValues[current_msaa_idx];
        saveSettings();
    }

    if (msaa_level != active_msaa_level) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                           "Restart required to apply MSAA changes.");
    }
}

void Gui::renderSettingsContent(Scene& scene, sf::Window& window) {
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
}

void Gui::renderFPSOverlay() const {
    if (!show_fps_overlay) return;

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(ImVec2(15.0f, 15.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    if (ImGui::Begin("FPSOverlay", nullptr, windowFlags)) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "FPS: %.0f", ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

void Gui::renderHUD(const std::string& formattedTime) const {
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoInputs;

    ImVec2 windowPos(ImGui::GetIO().DisplaySize.x * 0.5f, 30.0f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    if (ImGui::Begin("GameHUD", nullptr, windowFlags)) {
        ImGui::SetWindowFontScale(2.5f);
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", formattedTime.c_str());
        ImGui::SetWindowFontScale(1.0f);
    }
    ImGui::End();
}

void Gui::renderMinimapWindow(GLuint textureId) const {
    if (!minimap_enabled || textureId == 0) return;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                             ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    float padding = 20.0f;

    ImGui::SetNextWindowPos(ImVec2(displaySize.x - padding, padding), ImGuiCond_Always,
                            ImVec2(1.0f, 0.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    if (ImGui::Begin("MinimapHUD", nullptr, flags)) {
        float mapDisplaySize = std::clamp(displaySize.x * 0.28f, 160.0f, 400.0f);

        ImVec2 p_min = ImGui::GetCursorScreenPos();
        ImVec2 p_max(p_min.x + mapDisplaySize, p_min.y + mapDisplaySize);
        float rounding = ImGui::GetStyle().WindowRounding;

        ImGui::GetWindowDrawList()->AddImageRounded(textureId, p_min, p_max, ImVec2(0, 1),
                                                    ImVec2(1, 0), IM_COL32_WHITE, rounding);

        ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.85f, 0.70f, 0.20f, 0.8f));

        ImGui::GetWindowDrawList()->AddRect(ImVec2(p_min.x + 0.5f, p_min.y + 0.5f),
                                            ImVec2(p_max.x - 0.5f, p_max.y - 0.5f), borderColor,
                                            rounding, 0, 3.0f);

        ImGui::Dummy(ImVec2(mapDisplaySize, mapDisplaySize));
    }
    ImGui::End();

    ImGui::PopStyleVar(2);
}

void Gui::renderLeaderboardTable(const SessionManager& session, int targetDiff) const {
    auto formatDate = [](std::tm* local_tm) {
        return local_tm ? std::format("{:02}/{:02}/{} {:02}:{:02}:{:02}", local_tm->tm_mday,
                                      local_tm->tm_mon + 1, local_tm->tm_year + 1900,
                                      local_tm->tm_hour, local_tm->tm_min, local_tm->tm_sec)
                        : "Date not available";
    };

    if (ImGui::BeginChild("LeaderboardList", ImVec2(0, 250), true)) {
        auto allScores = session.getLeaderboard();
        std::vector<ScoreRecord> scores;

        for (const auto& s : allScores) {
            if (s.difficulty == targetDiff) {
                scores.push_back(s);
            }
            if (scores.size() == 50) break;
        }

        if (scores.empty()) {
            ImGui::Text("No scores yet. Play a game!");
        } else if (ImGui::BeginTable("LeaderboardTable", 4,
                                     ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                         ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 20.0f);
            ImGui::TableSetupColumn("Seed");
            ImGui::TableSetupColumn("Time");
            ImGui::TableSetupColumn("Date");
            ImGui::TableHeadersRow();

            unsigned long long index = 0;
            for (const auto& s : scores) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%llu", ++index);
                ImGui::TableNextColumn();
                ImGui::Text("%u", s.seed);
                ImGui::TableNextColumn();
                ImGui::Text("%s", s.timeStr.c_str());
                ImGui::TableNextColumn();

                auto time = static_cast<std::time_t>(s.timestamp);
                std::tm* local_tm = std::localtime(&time);  // NOSONAR
                ImGui::Text("%s", formatDate(local_tm).c_str());
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

void Gui::renderLeaderboardSection(const SessionManager& session) {
    ImGui::TextColored(ImVec4(0.2f, 0.70f, 1.0f, 1.0f), "Leaderboard");
    ImGui::Separator();

    if (ImGui::BeginTabBar("LeaderboardTabs")) {
        for (int i = 0; i < 4; ++i) {
            if (ImGui::BeginTabItem(diffNames[i])) {
                renderLeaderboardTable(session, i);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::Spacing();
    if (ImGui::Button("Back", ImVec2(-1.0f, 38.0f))) {
        showLeaderboard = false;
    }
}

void Gui::renderMainMenuSettingsSection(Scene& scene, sf::Window& window) {
    ImGui::TextColored(ImVec4(0.2f, 0.70f, 1.0f, 1.0f), "Settings");
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::BeginChild("SettingsContent", ImVec2(0, 250), false)) {
        renderSettingsContent(scene, window);
    }
    ImGui::EndChild();

    ImGui::Spacing();
    if (ImGui::Button("Back", ImVec2(-1.0f, 38.0f))) {
        showSettings = false;
    }
}

void Gui::renderMainMenuSection(Scene& scene, sf::Window& window, const SessionManager& session,
                                const GuiCallbacks& callbacks) {
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    float windowWidth = showSettings || showLeaderboard ? 520.0f : 400.0f;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, 0.0f), ImGuiCond_Always);

    ImGui::Begin("Tu Maze Main Menu", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings);

    if (showLeaderboard) {
        renderLeaderboardSection(session);
    } else if (showSettings) {
        renderMainMenuSettingsSection(scene, window);
    } else {
        ImGui::Text("Difficulty:");
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::Combo("##Difficulty", &selectedDifficulty, diffLabels.data(),
                         static_cast<int>(diffLabels.size()))) {
            saveSettings();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Play (Random Seed)", ImVec2(-1.0f, 38.0f)) && callbacks.onPlayRandom) {
            callbacks.onPlayRandom(selectedDifficulty);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Custom Seed:");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##CustomSeed", &customSeedInput, 0, 0, ImGuiInputTextFlags_CharsDecimal);
        if (ImGui::Button("Play (Custom Seed)", ImVec2(-1.0f, 38.0f)) && callbacks.onPlayCustom) {
            callbacks.onPlayCustom(static_cast<unsigned int>(std::max(0, customSeedInput)),
                                   selectedDifficulty);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Leaderboard", ImVec2(-1.0f, 38.0f))) {
            showLeaderboard = true;
        }
        if (ImGui::Button("Settings", ImVec2(-1.0f, 38.0f))) {
            showSettings = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Quit to Desktop", ImVec2(-1.0f, 38.0f)) && callbacks.onQuitDesktop) {
            callbacks.onQuitDesktop();
        }
    }
    ImGui::End();
}

void Gui::renderVictorySection(const GuiCallbacks& callbacks) const {
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin(
        "Victory!", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

    ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Congratulations!");
    ImGui::Text("You successfully navigated out of the maze.");
    ImGui::Separator();

    float buttonWidth = 140.0f;

    if (ImGui::Button("Main Menu", ImVec2(buttonWidth, 0)) && callbacks.onReturnToMenu) {
        callbacks.onReturnToMenu();
    }

    float rightAlignX = ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().WindowPadding.x;
    ImGui::SameLine(rightAlignX);

    if (ImGui::Button("Play Again", ImVec2(buttonWidth, 0)) && callbacks.onPlayAgain) {
        callbacks.onPlayAgain();
    }
    ImGui::End();
}

void Gui::renderPauseSection(Scene& scene, sf::Window& window, const GuiCallbacks& callbacks) {
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520.0f, 460.0f), ImGuiCond_Appearing);

    ImGuiWindowFlags pauseFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Game Paused", &paused, pauseFlags);

    if (ImGui::Button("Resume Game", ImVec2(-1.0f, 38.0f))) {
        paused = false;
    }
    if (ImGui::Button("Return to Main Menu", ImVec2(-1.0f, 38.0f)) && callbacks.onReturnToMenu) {
        callbacks.onReturnToMenu();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    renderSettingsContent(scene, window);

    ImGui::End();
}

int Gui::getSavedMSAA() {
    if (std::ifstream file(settingsFile); file.is_open()) {
        int q = 0;
        int m = 0;
        bool v = false;
        if (file >> q >> v >> m) {
            return m;
        }
    }
    return 0;
}

Gui::Gui(sf::Window& window) {
    if (!ImGui::SFML::Init(window, sf::Vector2f(window.getSize()), false)) {
        throw exceptions::GuiException("Error during ImGui-SFML initialization!");
    }

    ImGui_ImplOpenGL3_Init("#version 410 core");
    setupImGuiStyle();

    active_msaa_level = getSavedMSAA();
    loadSettings();

    window.setVerticalSyncEnabled(vsync_enabled);

    if (active_msaa_level > 0)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
}

Gui::~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::SFML::Shutdown();
}

void Gui::update(const sf::Window& window, sf::Time dt) const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::SFML::Update(sf::Mouse::getPosition(window), sf::Vector2f(window.getSize()), dt);
}

void Gui::renderUI(Scene& scene, sf::Window& window, const SessionManager& session,
                   const GuiCallbacks& callbacks, GLuint minimapTexture) {
    if (inMainMenu) {
        renderMainMenuSection(scene, window, session, callbacks);
    } else {
        if (won) {
            renderVictorySection(callbacks);
        }
        if (paused) {
            renderPauseSection(scene, window, callbacks);
        }
        if (!won && !paused) {
            renderHUD(session.getFormattedTime());
            renderMinimapWindow(minimapTexture);
        }
        renderFPSOverlay();
    }

    if (static bool isFirstFrame = true; isFirstFrame) {
        scene.getCamera().setFov(camera_fov);
        scene.getCamera().setMouseSensitivity(camera_sensitivity);
        scene.getCamera().setBobbingAmount(camera_bobbing);
        isFirstFrame = false;
    }

    ImGui::Render();

    // Standard Practice: Ensure MSAA is deactivated before drawing orthographic UI
    // to prevent bleeding edge artifacts on ImGui text rendering
    glDisable(GL_MULTISAMPLE);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
}

void Gui::renderLoading(const sf::Window& window, const std::string& what, float progress) const {
    ImVec2 windowSize(static_cast<float>(window.getSize().x),
                      static_cast<float>(window.getSize().y));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(windowSize);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs;

    ImGui::Begin("Loading", nullptr, flags);

    std::string text = "Loading " + what + "...";
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

    float barWidth = windowSize.x * 0.5f;
    float barHeight = 24.0f;
    float spacing = 12.0f;

    float totalContentHeight = textSize.y + spacing + barHeight;
    float startY = (windowSize.y - totalContentHeight) * 0.5f;

    ImGui::SetCursorPosY(startY);
    ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
    ImGui::Text("%s", text.c_str());

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.70f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + spacing);
    ImGui::SetCursorPosX((windowSize.x - barWidth) * 0.5f);
    ImGui::ProgressBar(progress, ImVec2(barWidth, barHeight));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Render();
    glDisable(GL_MULTISAMPLE);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
}
