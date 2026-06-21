#pragma once

#include <string>
#include <vector>

#include "glad/gl.h"

enum class TextureQuality : int { High = 0, Medium = 1, Low = 2 };

// Safe RAII wrapper for OpenGL Textures, managing global quality scaling and fallback logic
class Texture {
   private:
    GLuint m_textureID = 0;
    inline static std::vector<Texture*> instances;
    inline static GLfloat maxSupportedAnisotropy = -1.0f;
    inline static TextureQuality currentGlobalQuality = TextureQuality::Medium;

   public:
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    [[nodiscard]] static TextureQuality autoDetectQuality();

    explicit Texture(const std::string& filepath, bool sRGB = false);

    ~Texture();

    void setQuality(TextureQuality quality) const;

    [[nodiscard]] static TextureQuality getGlobalQuality() { return currentGlobalQuality; }

    static void setGlobalQuality(TextureQuality quality);

    void bind(unsigned int slot = 0) const;

    void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    [[nodiscard]] GLuint getID() const { return m_textureID; }
};
