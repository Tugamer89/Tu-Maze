#ifndef MINIMAP_HH
#define MINIMAP_HH

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

    // FBO Multisampled
    GLuint fbo = 0;
    GLuint rboColor = 0;
    GLuint rboDepth = 0;

    // FBO to resolve MSAA conflicts
    GLuint fboResolve = 0;
    GLuint texColorResolve = 0;

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

        // Apple Silicon / macOS compatible Depth+Stencil format. GL_DEPTH_COMPONENT24 can fail
        // completeness tests.
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, actualSamples, GL_DEPTH24_STENCIL8,
                                         renderResolution, renderResolution);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  rboDepth);

        // Setup Resolve FBO (Non-MSAA)
        glBindFramebuffer(GL_FRAMEBUFFER, fboResolve);
        glBindTexture(GL_TEXTURE_2D, texColorResolve);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderResolution, renderResolution, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);

        // ImGui texture properties guarantee
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorResolve,
                               0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    GLuint getTextureID() const { return texColorResolve; }

    void draw(const Scene& scene, const Gui& gui) {
        if (!gui.minimap_enabled) return;

        initFBO();

        // Save old MSAA status
        GLboolean wasMSAA;
        glGetBooleanv(GL_MULTISAMPLE, &wasMSAA);
        glEnable(GL_MULTISAMPLE);

        // Save old Viewport status to avoid shrinking the main game
        std::array<GLint, 4> last_viewport{};
        glGetIntegerv(GL_VIEWPORT, last_viewport.data());

        // Setup Minimap Target
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, renderResolution, renderResolution);
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);  // Map background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders.use();

        // Render Scene & Player
        glm::vec3 camPos = scene.camera.getPosition();
        setupProjection(scene, gui, camPos);

        GLint modelLoc = glGetUniformLocation(shaders.program, "model");
        GLint colorLoc = glGetUniformLocation(shaders.program, "color");

        float cullRadius = gui.minimap_zoom * 1.5f;
        scene.root.drawMinimap(modelLoc, colorLoc, camPos, cullRadius);
        scene.goalNode.drawMinimap(modelLoc, colorLoc, camPos, cullRadius);
        drawPlayerMarker(scene, gui, modelLoc, colorLoc, camPos);

        // Resolve MSAA from FBO Multisampled directly to Texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboResolve);
        glBlitFramebuffer(0, 0, renderResolution, renderResolution, 0, 0, renderResolution,
                          renderResolution, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // End operation and restore original state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);

        if (!wasMSAA) glDisable(GL_MULTISAMPLE);
    }
};

#endif
