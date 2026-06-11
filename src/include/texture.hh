#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <SFML/Graphics/Image.hpp>
#include <iostream>
#include <string>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

enum class TextureQuality : int { High = 0, Medium = 1, Low = 2 };

class Texture {
   private:
    GLuint m_textureID = 0;
    inline static std::vector<Texture*> instances;
    inline static GLfloat maxSupportedAnisotropy = -1.0f;

   public:
    inline static TextureQuality currentGlobalQuality = TextureQuality::Medium;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    static TextureQuality autoDetectQuality() {
        using enum TextureQuality;

        if (maxSupportedAnisotropy < 0.0f) {
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxSupportedAnisotropy);
        }

        // Simple hardware probe: modern dedicated GPUs support 16x Anisotropy,
        // while integrated/older GPUs often cap at 2x or 4x.

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
            // RAII standard: generate a 1x1 magenta fallback texture so the engine doesn't crash
            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);
            unsigned char magenta[] = {255, 0, 255, 255};  // NOSONAR

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);

            instances.push_back(this);
            setQuality(currentGlobalQuality);

            glBindTexture(GL_TEXTURE_2D, 0);
            return;
        }

        // OpenGL expects the Y-axis to go upwards, SFML goes downwards
        image.flipVertically();

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // Set wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Determine internal format based on whether it's an albedo texture (needs gamma
        // correction)
        GLint internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getSize().x, image.getSize().y, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);

        // Track instance and apply initial quality settings
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

    // Adjust Texture Base Level (forces GPU to use a smaller mipmap scale) & Filter
    void setQuality(TextureQuality quality) const {
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        switch (quality) {
            case TextureQuality::High: {
                GLfloat aniso = std::min(16.0f, maxSupportedAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR);                          // Minification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Magnification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                break;
            }
            case TextureQuality::Medium: {
                GLfloat aniso = std::min(4.0f, maxSupportedAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_NEAREST);                         // Minification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Magnification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 1);          // 1/2 resolution
                break;
            }
            case TextureQuality::Low: {
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                                1.0f);  // Disable anisotropic filter
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_NEAREST);                          // Minification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Magnification
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 2);  // 1/4 resolution
                break;
            }
        }
    }

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

    GLuint getID() const { return m_textureID; }
};

#endif
