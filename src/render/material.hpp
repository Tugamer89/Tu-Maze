#pragma once

#include <glm/vec3.hpp>

#include "glad/gl.h"
#include "render/texture.hpp"

struct MaterialLocations {
    GLint diffuse_loc = -1;
    GLint ambient_loc = -1;
    GLint specular_loc = -1;
    GLint shininess_loc = -1;
    GLint alpha_loc = -1;
    GLint useTextures_loc = -1;
    GLint useFlatShading_loc = -1;
};

// Represents physical surface properties mapping directly to the shader implementation
struct Material {
    Texture* diffuse = nullptr;
    Texture* normal = nullptr;
    Texture* roughness = nullptr;

    glm::vec3 diffuse_color = {1.0f, 1.0f, 1.0f};
    glm::vec3 ambient_color = {1.0f, 1.0f, 1.0f};
    glm::vec3 specular_color = {1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;
    float alpha = 1.0f;

    bool use_textures = true;
    bool use_flat = false;

    void bind(const MaterialLocations& locs) const;
    void unbind() const;
};
