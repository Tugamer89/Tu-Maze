#ifndef GUI_HH
#define GUI_HH

#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "scene.hh"
#include "texture.hh"

class Gui {
   private:
    const std::string settingsFile = "tu-maze_settings.txt";

    void loadSettings() const {
        std::ifstream file(settingsFile);
        if (file.is_open()) {
            if (int q; file >> q) {
                // Clamp loaded value to valid enum range to be safe
                q = std::clamp(q, static_cast<int>(TextureQuality::High),
                               static_cast<int>(TextureQuality::Low));
                Texture::currentGlobalQuality = static_cast<TextureQuality>(q);
            }
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
            file.close();
        }
    }

   public:
    explicit Gui(sf::Window& window) {
        if (!ImGui::SFML::Init(window, sf::Vector2f(window.getSize()), false)) {
            std::cerr << "Error during ImGui-SFML initialization!" << std::endl;
            exit(1);
        }
        // Start the native OpenGL 3 backend for GUI rendering
        ImGui_ImplOpenGL3_Init("#version 410 core");

        loadSettings();
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

    // Defines the interface and renders it
    void render(Scene& scene) const {
        ImGui::Begin("Settings");

        ImGui::Text("Performance & Quality:");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        auto currentQuality = static_cast<int>(Texture::currentGlobalQuality);
        if (std::array<const char*, 3> qualities = {"High", "Medium", "Low"};
            ImGui::Combo("Texture Quality", &currentQuality, qualities.data(), qualities.size())) {
            Texture::setGlobalQuality(static_cast<TextureQuality>(currentQuality));
            saveSettings();
        }

        ImGui::Separator();
        ImGui::Text("Modify world parameters:");

        bool updateShader = false;
        updateShader |= ImGui::ColorEdit3("Directional Light", &scene.lights.light_direct_val[0]);
        updateShader |= ImGui::ColorEdit3("Ambient Light", &scene.lights.light_ambient_val[0]);
        updateShader |= ImGui::ColorEdit3("Diffuse Color", &scene.lights.material_diffuse[0]);
        updateShader |= ImGui::ColorEdit3("Ambient Color", &scene.lights.material_ambient[0]);
        updateShader |=
            ImGui::SliderFloat("Shininess", &scene.lights.material_shininess, 1.0f, 2000.0f);

        bool updatePos =
            ImGui::SliderFloat3("Torch Position", &scene.lights.light_direct_pos[0], -2.0f, 2.0f);

        ImGui::End();

        // if the torch position changed, update the shader with the new position in view space
        if (updatePos) scene.lights.position(scene.camera.inv_v);

        // If a parameter changed, update the scene shaders
        if (updateShader) {
            scene.lights.parameters();
        }

        // Generate draw data and use OpenGL3 to render them
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

        // Push modern styles for the progress bar
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                              ImVec4(0.2f, 0.6f, 1.0f, 1.0f));  // Bright modern blue
        ImGui::PushStyleColor(ImGuiCol_FrameBg,
                              ImVec4(0.15f, 0.15f, 0.15f, 1.0f));  // Dark track background
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);    // Rounded edges

        // Render Centered Progress Bar
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
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif
