#ifndef GUI_HH
#define GUI_HH

#include <imgui-SFML.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "scene.hh"

class Gui {
   public:
    explicit Gui(sf::Window& window) {
        if (!ImGui::SFML::Init(window, sf::Vector2f(window.getSize()), false)) {
            std::cerr << "Error during ImGui-SFML initialization!" << std::endl;
            exit(1);
        }
        // Start the native OpenGL 3 backend for GUI rendering
        ImGui_ImplOpenGL3_Init("#version 410 core");
    }

    ~Gui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::SFML::Shutdown();
    }

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
        ImGui::Begin("Scene Settings");
        ImGui::Text("Modify world parameters:");

        bool updateShader = false;
        updateShader |= ImGui::ColorEdit3("Directional Light", &scene.lights.light_direct_val[0]);
        updateShader |= ImGui::ColorEdit3("Ambient Light", &scene.lights.light_ambient_val[0]);
        updateShader |= ImGui::ColorEdit3("Diffuse Color", &scene.lights.material_diffuse[0]);
        updateShader |= ImGui::ColorEdit3("Ambient Color", &scene.lights.material_ambient[0]);
        updateShader |=
            ImGui::SliderFloat("Shininess", &scene.lights.material_shininess, 1.0f, 2000.0f);

        ImGui::End();

        // If a parameter changed, update the scene shaders
        if (updateShader) {
            scene.lights.parameters();
        }

        // Generate draw data and use OpenGL3 to render them
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif
