#include "ui/gui.hpp"

#include <imgui_impl_opengl3.h>

#include <SFML/Window.hpp>
#include <algorithm>
#include <array>
#include <fstream>

#include "core/exceptions.hpp"

namespace {
constexpr std::array<const char*, 4> diffLabels = {
    "Easy (10x10)",
    "Normal (15x15)",
    "Hard (25x25)",
    "Extreme (40x40)",
};
constexpr std::array<const char*, 4> diffNames = {
    "Easy",
    "Normal",
    "Hard",
    "Extreme",
};
constexpr std::array<const char*, 3> qualities = {
    "High",
    "Medium",
    "Low",
};
constexpr std::array<const char*, 5> msaaLabels = {
    "Off (Faster)", "2x", "4x", "8x", "16x (Max Quality)",
};
constexpr std::array<int, 5> msaaValues = {0, 2, 4, 8, 16};
}  // namespace

// -------------------------------------------------------------------------------------------------
// GuiState Implementation
// -------------------------------------------------------------------------------------------------

void GuiState::loadSettings() {
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

void GuiState::saveSettings() const {
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

int GuiState::getSavedMSAA() {
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

// -------------------------------------------------------------------------------------------------
// GuiMenus Implementation
// -------------------------------------------------------------------------------------------------

void GuiMenus::renderControlsSection(GuiState& state, Scene& scene) {
    if (ImGui::SliderFloat("Mouse Sensitivity", &state.camera_sensitivity, 0.01f, 1.0f, "%.2f")) {
        scene.getCamera().setMouseSensitivity(state.camera_sensitivity);
        state.saveSettings();
    }

    if (ImGui::SliderFloat("Head Bobbing", &state.camera_bobbing, 0.0f, 0.05f, "%.3f")) {
        scene.getCamera().setBobbingAmount(state.camera_bobbing);
        state.saveSettings();
    }

    if (ImGui::SliderFloat("Field of View", &state.camera_fov, 30.0f, 120.0f, "%.1f deg")) {
        scene.getCamera().setFov(state.camera_fov);
        state.saveSettings();
    }
}

void GuiMenus::renderMinimapSection(GuiState& state) {
    if (ImGui::Checkbox("Show Minimap", &state.minimap_enabled)) state.saveSettings();

    if (state.minimap_enabled) {
        if (ImGui::Checkbox("Fix North", &state.minimap_fix_north)) state.saveSettings();
        if (ImGui::SliderFloat("Zoom", &state.minimap_zoom, 3.0f, 30.0f, "%.1f units"))
            state.saveSettings();
    }
}

void GuiMenus::renderVideoSection(GuiState& state, sf::Window& window) {
    if (ImGui::Checkbox("Show FPS Overlay", &state.show_fps_overlay)) {
        state.saveSettings();
    }

    ImGui::Separator();

    if (auto currentQuality = static_cast<int>(Texture::getGlobalQuality());
        ImGui::Combo("Texture Quality", &currentQuality, qualities.data(),
                     static_cast<int>(qualities.size()))) {
        Texture::setGlobalQuality(static_cast<TextureQuality>(currentQuality));
        state.saveSettings();
    }

    if (ImGui::Checkbox("V-Sync", &state.vsync_enabled)) {
        window.setVerticalSyncEnabled(state.vsync_enabled);
        state.saveSettings();
    }

    int current_msaa_idx = 0;
    for (size_t i = 0; i < msaaValues.size(); ++i) {
        if (state.msaa_level == msaaValues[i]) current_msaa_idx = static_cast<int>(i);
    }

    if (ImGui::Combo("Anti-Aliasing (MSAA)", &current_msaa_idx, msaaLabels.data(),
                     static_cast<int>(msaaLabels.size()))) {
        state.msaa_level = msaaValues[current_msaa_idx];
        state.saveSettings();
    }

    if (state.msaa_level != state.active_msaa_level) {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                           "Restart required to apply MSAA changes.");
    }
}

void GuiMenus::renderSettingsContent(GuiState& state, Scene& scene, sf::Window& window) {
    if (ImGui::BeginTabBar("SettingsTabs")) {
        if (ImGui::BeginTabItem("Video & Display")) {
            ImGui::Spacing();
            renderVideoSection(state, window);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Controls & Camera")) {
            ImGui::Spacing();
            renderControlsSection(state, scene);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Minimap")) {
            ImGui::Spacing();
            renderMinimapSection(state);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GuiMenus::renderLeaderboardTable(const SessionManager& session, int targetDiff) {
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

void GuiMenus::renderLeaderboardSection(GuiState& state, const SessionManager& session) {
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
        state.showLeaderboard = false;
    }
}

void GuiMenus::renderMainMenu(GuiState& state, Scene& scene, sf::Window& window,
                              const SessionManager& session, const GuiCallbacks& callbacks) {
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    float windowWidth = state.showSettings || state.showLeaderboard ? 520.0f : 400.0f;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, 0.0f), ImGuiCond_Always);

    ImGui::Begin("Tu Maze Main Menu", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings);

    if (state.showLeaderboard) {
        renderLeaderboardSection(state, session);
    } else if (state.showSettings) {
        ImGui::TextColored(ImVec4(0.2f, 0.70f, 1.0f, 1.0f), "Settings");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginChild("SettingsContent", ImVec2(0, 250), false)) {
            renderSettingsContent(state, scene, window);
        }
        ImGui::EndChild();

        ImGui::Spacing();
        if (ImGui::Button("Back", ImVec2(-1.0f, 38.0f))) {
            state.showSettings = false;
        }
    } else {
        ImGui::Text("Difficulty:");
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::Combo("##Difficulty", &state.selectedDifficulty, diffLabels.data(),
                         static_cast<int>(diffLabels.size()))) {
            state.saveSettings();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Play (Random Seed)", ImVec2(-1.0f, 38.0f)) && callbacks.onPlayRandom) {
            callbacks.onPlayRandom(state.selectedDifficulty);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Custom Seed:");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##CustomSeed", &state.customSeedInput, 0, 0,
                        ImGuiInputTextFlags_CharsDecimal);
        if (ImGui::Button("Play (Custom Seed)", ImVec2(-1.0f, 38.0f)) && callbacks.onPlayCustom) {
            callbacks.onPlayCustom(static_cast<unsigned int>(std::max(0, state.customSeedInput)),
                                   state.selectedDifficulty);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Leaderboard", ImVec2(-1.0f, 38.0f))) {
            state.showLeaderboard = true;
        }
        if (ImGui::Button("Settings", ImVec2(-1.0f, 38.0f))) {
            state.showSettings = true;
        }

        ImGui::Spacing();
        if (ImGui::Button("Quit to Desktop", ImVec2(-1.0f, 38.0f)) && callbacks.onQuitDesktop) {
            callbacks.onQuitDesktop();
        }
    }
    ImGui::End();
}

void GuiMenus::renderPauseMenu(GuiState& state, Scene& scene, sf::Window& window,
                               const GuiCallbacks& callbacks) {
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(520.0f, 460.0f), ImGuiCond_Appearing);

    ImGuiWindowFlags pauseFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    ImGui::Begin("Game Paused", &state.paused, pauseFlags);

    if (ImGui::Button("Resume Game", ImVec2(-1.0f, 38.0f))) {
        state.paused = false;
    }
    if (ImGui::Button("Return to Main Menu", ImVec2(-1.0f, 38.0f)) && callbacks.onReturnToMenu) {
        callbacks.onReturnToMenu();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    renderSettingsContent(state, scene, window);

    ImGui::End();
}

void GuiMenus::renderVictory(const GuiCallbacks& callbacks) {
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

// -------------------------------------------------------------------------------------------------
// GuiOverlays Implementation
// -------------------------------------------------------------------------------------------------

void GuiOverlays::renderFPS(bool show) {
    if (!show) return;

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

void GuiOverlays::renderHUD(const std::string& formattedTime) {
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

void GuiOverlays::renderMinimap(bool enabled, GLuint textureId) {
    if (!enabled || textureId == 0) return;

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

void GuiOverlays::renderLoading(const sf::Window& window, const std::string& what, float progress) {
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
}

// -------------------------------------------------------------------------------------------------
// Main Gui Implementation
// -------------------------------------------------------------------------------------------------

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

Gui::Gui(sf::Window& window) {
    if (!ImGui::SFML::Init(window, sf::Vector2f(window.getSize()), false)) {
        throw exceptions::GuiException("Error during ImGui-SFML initialization!");
    }

    ImGui_ImplOpenGL3_Init("#version 410 core");
    setupImGuiStyle();

    state.active_msaa_level = GuiState::getSavedMSAA();
    state.loadSettings();

    window.setVerticalSyncEnabled(state.vsync_enabled);

    if (state.active_msaa_level > 0)
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
    if (state.inMainMenu) {
        GuiMenus::renderMainMenu(state, scene, window, session, callbacks);
    } else {
        if (state.won) {
            GuiMenus::renderVictory(callbacks);
        }
        if (state.paused) {
            GuiMenus::renderPauseMenu(state, scene, window, callbacks);
        }
        if (!state.won && !state.paused) {
            GuiOverlays::renderHUD(session.getFormattedTime());
            GuiOverlays::renderMinimap(state.minimap_enabled, minimapTexture);
        }
        GuiOverlays::renderFPS(state.show_fps_overlay);
    }

    if (static bool isFirstFrame = true; isFirstFrame) {
        scene.getCamera().setFov(state.camera_fov);
        scene.getCamera().setMouseSensitivity(state.camera_sensitivity);
        scene.getCamera().setBobbingAmount(state.camera_bobbing);
        isFirstFrame = false;
    }

    ImGui::Render();

    glDisable(GL_MULTISAMPLE);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (state.active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
}

void Gui::renderLoading(const sf::Window& window, const std::string& what, float progress) const {
    GuiOverlays::renderLoading(window, what, progress);

    if (state.active_msaa_level > 0) glEnable(GL_MULTISAMPLE);
}
