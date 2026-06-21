#pragma once

#include <algorithm>
#include <array>
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
    Shaders shaders;

    // Primary Multisampled Framebuffer Object for rendering smooth map outlines
    GLuint fbo = 0;
    GLuint rboColor = 0;
    GLuint rboDepth = 0;

    // Secondary Resolve Framebuffer Object because MSAA textures cannot be drawn directly by ImGui
    GLuint fboResolve = 0;
    GLuint texColorResolve = 0;

    const GPUMesh* playerMesh = nullptr;

    const int renderResolution = 512;
    const int msaaSamples = 8;

    void initFBO() {
        if (fbo != 0) return;

        GLint maxSamples = 0;
        glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
        int actualSamples = std::min(msaaSamples, maxSamples);

        glGenFramebuffers(1, &fbo);
        glGenRenderbuffers(1, &rboColor);
        glGenRenderbuffers(1, &rboDepth);

        glGenFramebuffers(1, &fboResolve);
        glGenTextures(1, &texColorResolve);

        // Setup MSAA FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glBindRenderbuffer(GL_RENDERBUFFER, rboColor);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, actualSamples, GL_RGBA8, renderResolution,
                                         renderResolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColor);

        // Uses universally compatible Depth+Stencil format for complex hardware (e.g., Apple
        // Silicon)
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, actualSamples, GL_DEPTH24_STENCIL8,
                                         renderResolution, renderResolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  rboDepth);

        // Setup Resolve FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fboResolve);
        glBindTexture(GL_TEXTURE_2D, texColorResolve);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderResolution, renderResolution, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorResolve,
                               0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setupProjection(const Scene& scene, const Gui& gui, const glm::vec3& camPos) const {
        // Orthographic projection provides a flat, 2D top-down view
        glm::mat4 pr = glm::ortho(-gui.getMinimapZoom(), gui.getMinimapZoom(),
                                  -gui.getMinimapZoom(), gui.getMinimapZoom(), 0.1f, 100.0f);

        glm::vec3 up;
        if (gui.isMinimapFixNorth()) {
            up = glm::vec3(0.0f, 0.0f, -1.0f);
        } else {
            glm::vec3 front = scene.getCamera().getFront();
            up = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
        }

        glm::mat4 view = glm::lookAt(camPos + glm::vec3(0.0f, 40.0f, 0.0f), camPos, up);
        glm::mat4 vp = pr * view;

        glUniformMatrix4fv(glGetUniformLocation(shaders.getProgram(), "vp"), 1, GL_FALSE,
                           glm::value_ptr(vp));
    }

    void drawPlayerMarker(const Scene& scene, const Gui& gui, GLint modelLoc, GLint colorLoc,
                          const glm::vec3& camPos) const {
        if (!playerMesh) return;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), camPos);
        model = glm::rotate(model, glm::radians(-scene.getCamera().getYaw() - 90.0f),
                            glm::vec3(0, 1, 0));

        float markerScale = gui.getMinimapZoom() * 0.15f;
        model = glm::scale(model, glm::vec3(markerScale));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 1.0f, 0.2f, 0.2f);

        glDisable(GL_DEPTH_TEST);
        playerMesh->draw();
        glEnable(GL_DEPTH_TEST);
    }

   public:
    Minimap(const std::string& vert_path, const std::string& frag_path)
        : shaders(vert_path, frag_path) {}

    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;
    Minimap(Minimap&&) = delete;
    Minimap& operator=(Minimap&&) = delete;

    ~Minimap() {
        if (fbo != 0) glDeleteFramebuffers(1, &fbo);
        if (rboColor != 0) glDeleteRenderbuffers(1, &rboColor);
        if (rboDepth != 0) glDeleteRenderbuffers(1, &rboDepth);

        if (fboResolve != 0) glDeleteFramebuffers(1, &fboResolve);
        if (texColorResolve != 0) glDeleteTextures(1, &texColorResolve);
    }

    void setPlayerMesh(const GPUMesh* mesh) { playerMesh = mesh; }

    void reloadShaders(const std::string& vert_path, const std::string& frag_path) {
        shaders.reload(vert_path, frag_path);
    }

    [[nodiscard]] GLuint getTextureID() const { return texColorResolve; }

    void draw(const Scene& scene, const Gui& gui) {
        if (!gui.isMinimapEnabled()) return;

        initFBO();

        // State machine preservation to avoid corrupting the main 3D scene pass
        GLboolean wasMSAA;
        glGetBooleanv(GL_MULTISAMPLE, &wasMSAA);
        glEnable(GL_MULTISAMPLE);

        std::array<GLint, 4> last_viewport{};
        glGetIntegerv(GL_VIEWPORT, last_viewport.data());

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, renderResolution, renderResolution);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders.use();

        glm::vec3 camPos = scene.getCamera().getPosition();
        setupProjection(scene, gui, camPos);

        GLint modelLoc = glGetUniformLocation(shaders.getProgram(), "model");
        GLint colorLoc = glGetUniformLocation(shaders.getProgram(), "color");

        // Radially cull minimap geometry that exists beyond the map zoom
        float cullRadius = gui.getMinimapZoom() * 1.5f;
        scene.getRoot().drawMinimap(modelLoc, colorLoc, camPos, cullRadius);
        scene.getGoalNode().drawMinimap(modelLoc, colorLoc, camPos, cullRadius);
        drawPlayerMarker(scene, gui, modelLoc, colorLoc, camPos);

        // Blit resolves the Multisampled FBO into a readable 2D Texture for ImGui
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboResolve);
        glBlitFramebuffer(0, 0, renderResolution, renderResolution, 0, 0, renderResolution,
                          renderResolution, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);

        if (!wasMSAA) glDisable(GL_MULTISAMPLE);
    }
};
