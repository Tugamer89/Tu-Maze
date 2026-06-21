#pragma once

#include <SFML/Graphics/Image.hpp>
#include <array>
#include <iostream>
#include <string>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

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

    [[nodiscard]] static TextureQuality autoDetectQuality() {
        using enum TextureQuality;

        // Query the driver to determine Anisotropic support levels
        if (maxSupportedAnisotropy < 0.0f) {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxSupportedAnisotropy);
        }

        if (maxSupportedAnisotropy >= 8.0f) return High;
        if (maxSupportedAnisotropy >= 4.0f) return Medium;
        return Low;
    }

    explicit Texture(const std::string& filepath, bool sRGB = false) {
        if (maxSupportedAnisotropy < 0.0f) {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxSupportedAnisotropy);
        }

        sf::Image image;
        if (!image.loadFromFile(filepath)) {
            std::cerr << "[Warning] Failed to load texture: " << filepath
                      << ". Generating fallback.\n";

            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);

            // Standard missing-texture fallback to prevent silent geometry blackouts
            constexpr std::array<uint8_t, 4> magenta = {255, 0, 255, 255};

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         magenta.data());

            instances.push_back(this);
            setQuality(currentGlobalQuality);

            glBindTexture(GL_TEXTURE_2D, 0);
            return;
        }

        image.flipVertically();

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // sRGB informs the GPU to perform gamma correction upon sampling
        GLint internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getSize().x, image.getSize().y, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);

        instances.push_back(this);
        setQuality(currentGlobalQuality);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture() {
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
        }
        std::erase(instances, this);
    }

    void setQuality(TextureQuality quality) const {
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        switch (quality) {
            case TextureQuality::High: {
                GLfloat aniso = std::min(16.0f, maxSupportedAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                break;
            }
            case TextureQuality::Medium: {
                GLfloat aniso = std::min(4.0f, maxSupportedAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Forces GPU to start sampling from Mip level 1 (Half resolution)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 1);
                break;
            }
            case TextureQuality::Low: {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                // Forces GPU to start sampling from Mip level 2 (Quarter resolution)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 2);
                break;
            }
        }
    }

    [[nodiscard]] static TextureQuality getGlobalQuality() { return currentGlobalQuality; }

    static void setGlobalQuality(TextureQuality quality) {
        currentGlobalQuality = quality;
        for (auto const* tex : instances) {
            tex->setQuality(quality);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }

    void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    [[nodiscard]] GLuint getID() const { return m_textureID; }
};
