#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <SFML/Graphics/Image.hpp>
#include <string>
#include <iostream>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

class Texture {
private:
    GLuint m_textureID = 0;

public:
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    explicit Texture(const std::string& filepath, bool sRGB = false) {
        sf::Image image;
        if (!image.loadFromFile(filepath)) {
            std::cerr << "[Warning] Failed to load texture: " << filepath << ". Generating fallback.\n";
            // RAII standard: generate a 1x1 magenta fallback texture so the engine doesn't crash
            glGenTextures(1, &m_textureID);
            glBindTexture(GL_TEXTURE_2D, m_textureID);
            unsigned char magenta[] = { 255, 0, 255, 255 };  // NOSONAR
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
            glBindTexture(GL_TEXTURE_2D, 0);
            return;
        }

        // OpenGL expects the Y-axis to go upwards, SFML goes downwards
        image.flipVertically();

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);

        // Set wrapping and high-quality filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Determine internal format based on whether it's an albedo texture (needs gamma correction)
        GLint internalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.getSize().x, image.getSize().y, 
                        0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture() {
        if (m_textureID != 0) {
            glDeleteTextures(1, &m_textureID);
        }
    }

    void bind(unsigned int slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
    }
    
    void unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint getID() const { return m_textureID; }
};

#endif
