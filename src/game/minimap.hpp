#pragma once

#include "core/scene.hpp"
#include "glad/gl.h"
#include "render/gpumesh.hpp"
#include "render/hotshaders.hpp"
#include "ui/gui.hpp"

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

    GLint modelLoc = -1;
    GLint colorLoc = -1;
    GLint vpLoc = -1;

    void updateUniformLocations();
    void initFBO();

    void setupProjection(const Scene& scene, const Gui& gui, const glm::vec3& camPos) const;

    void drawPlayerMarker(const Scene& scene, const Gui& gui, GLint mLoc, GLint cLoc,
                          const glm::vec3& camPos) const;

   public:
    Minimap(const std::string& vert_path, const std::string& frag_path)
        : shaders(vert_path, frag_path) {
        updateUniformLocations();
    }

    Minimap(const Minimap&) = delete;
    Minimap& operator=(const Minimap&) = delete;
    Minimap(Minimap&&) = delete;
    Minimap& operator=(Minimap&&) = delete;

    ~Minimap();

    void setPlayerMesh(const GPUMesh* mesh) { playerMesh = mesh; }

    void reloadShaders(const std::string& vert_path, const std::string& frag_path) {
        shaders.reload(vert_path, frag_path);
        updateUniformLocations();
    }

    [[nodiscard]] GLuint getTextureID() const { return texColorResolve; }

    void draw(const Scene& scene, const Gui& gui);
};
