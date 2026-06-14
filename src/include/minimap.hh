#ifndef MINIMAP_HH
#define MINIMAP_HH

#include <SFML/Window.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "gpumesh.hh"
#include "gui.hh"
#include "hotshaders.hh"
#include "scene.hh"

class Minimap {
   private:
    struct MinimapMetrics {
        int size;
        int x;
        int y;
        int windowWidth;
        int windowHeight;
    };

    Shaders shaders;

    // FBO Multisampled
    GLuint fbo = 0;
    GLuint rboColor = 0;
    GLuint rboDepth = 0;

    // FBO to resolve MSAA conflicts
    GLuint fboResolve = 0;
    GLuint texColorResolve = 0;

    int currentSize = 0;
    const int msaaSamples = 8;

    void resizeFBO(int newSize) {
        if (fbo == 0) {
            glGenFramebuffers(1, &fbo);
            glGenRenderbuffers(1, &rboColor);
            glGenRenderbuffers(1, &rboDepth);

            glGenFramebuffers(1, &fboResolve);
            glGenTextures(1, &texColorResolve);
        }

        // Setup MSAA FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGB8, newSize, newSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);

        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH_COMPONENT24,
                                         newSize, newSize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        // Setup Resolve FBO (Non-MSAA)
        glBindFramebuffer(GL_FRAMEBUFFER, fboResolve);
        glBindTexture(GL_TEXTURE_2D, texColorResolve);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, newSize, newSize, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorResolve,
                               0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        currentSize = newSize;
    }

    MinimapMetrics calculateMetrics(const sf::Window& window) const {
        auto wWidth = static_cast<int>(window.getSize().x);
        auto wHeight = static_cast<int>(window.getSize().y);

        // Clamp size between 150 and 300
        int size = std::clamp(wWidth / 4, 150, 300);
        int margin = 20;

        return {size, wWidth - size - margin, wHeight - size - margin, wWidth, wHeight};
    }

    void drawBorder(const MinimapMetrics& metrics) const {
        glEnable(GL_SCISSOR_TEST);
        glScissor(metrics.x - 3, metrics.y - 3, metrics.size + 6, metrics.size + 6);
        glClearColor(0.85f, 0.70f, 0.20f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }

    void bindAndClearFBO(const MinimapMetrics& metrics) const {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, metrics.size, metrics.size);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void setupProjection(const Scene& scene, const Gui& gui, const glm::vec3& camPos) const {
        glm::mat4 pr = glm::ortho(-gui.minimap_zoom, gui.minimap_zoom, -gui.minimap_zoom,
                                  gui.minimap_zoom, 0.1f, 100.0f);

        glm::vec3 up;
        if (gui.minimap_fix_north) {
            up = glm::vec3(0.0f, 0.0f, -1.0f);  // North is -Z
        } else {
            glm::vec3 front = scene.camera.getFront();
            up = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        }

        glm::mat4 view = glm::lookAt(camPos + glm::vec3(0.0f, 40.0f, 0.0f), camPos, up);
        glm::mat4 vp = pr * view;

        glUniformMatrix4fv(glGetUniformLocation(shaders.program, "vp"), 1, GL_FALSE,
                           glm::value_ptr(vp));
    }

    void drawPlayerMarker(const Scene& scene, const Gui& gui, GLint modelLoc, GLint colorLoc,
                          const glm::vec3& camPos) const {
        if (!playerMesh) return;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), camPos);
        model =
            glm::rotate(model, glm::radians(-scene.camera.getYaw() - 90.0f), glm::vec3(0, 1, 0));

        float markerScale = gui.minimap_zoom * 0.15f;
        model = glm::scale(model, glm::vec3(markerScale));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 1.0f, 0.2f, 0.2f);  // Bright red

        glDisable(GL_DEPTH_TEST);  // Always on top
        playerMesh->draw();
        glEnable(GL_DEPTH_TEST);
    }

    void blitToScreenAndRestore(const MinimapMetrics& metrics) const {
        // Resolve MSAA from FBO Multisampled
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboResolve);
        glBlitFramebuffer(0, 0, metrics.size, metrics.size, 0, 0, metrics.size, metrics.size,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Copy FBO Resolved to Main Framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboResolve);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, metrics.size, metrics.size, metrics.x, metrics.y,
                          metrics.x + metrics.size, metrics.y + metrics.size, GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        // Restore main Viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, metrics.windowWidth, metrics.windowHeight);
    }

   public:
    const GPUMesh* playerMesh = nullptr;

    Minimap(const std::string& vert_path, const std::string& frag_path)
        : shaders(vert_path, frag_path) {}

    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;

    ~Minimap() {
        if (fbo != 0) glDeleteFramebuffers(1, &fbo);
        if (rboColor != 0) glDeleteRenderbuffers(1, &rboColor);
        if (rboDepth != 0) glDeleteRenderbuffers(1, &rboDepth);

        if (fboResolve != 0) glDeleteFramebuffers(1, &fboResolve);
        if (texColorResolve != 0) glDeleteTextures(1, &texColorResolve);
    }

    void reloadShaders(const std::string& vert_path, const std::string& frag_path) {
        shaders.reload(vert_path, frag_path);
    }

    void draw(const Scene& scene, const Gui& gui, const sf::Window& window) {
        if (!gui.minimap_enabled) return;

        MinimapMetrics metrics = calculateMetrics(window);

        if (metrics.size != currentSize) {
            resizeFBO(metrics.size);
        }

        drawBorder(metrics);

        // Save old MSAA status
        GLboolean wasMSAA;
        glGetBooleanv(GL_MULTISAMPLE, &wasMSAA);
        glEnable(GL_MULTISAMPLE);

        // Setup Minimap Target
        bindAndClearFBO(metrics);
        shaders.use();

        // Render Scene & Player
        glm::vec3 camPos = scene.camera.getPosition();
        setupProjection(scene, gui, camPos);

        GLint modelLoc = glGetUniformLocation(shaders.program, "model");
        GLint colorLoc = glGetUniformLocation(shaders.program, "color");

        float cullRadius = gui.minimap_zoom * 1.5f;
        scene.root.drawMinimap(modelLoc, colorLoc, camPos, cullRadius);
        drawPlayerMarker(scene, gui, modelLoc, colorLoc, camPos);

        // Resolve to screen
        blitToScreenAndRestore(metrics);

        if (!wasMSAA) glDisable(GL_MULTISAMPLE);
    }
};

#endif
