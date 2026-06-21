#pragma once

#include <glm/vec3.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "render/texture.hh"

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

    void bind(const MaterialLocations& locs) const {
        if (diffuse) diffuse->bind(0);
        if (normal) normal->bind(1);
        if (roughness) roughness->bind(2);

        if (locs.diffuse_loc != -1) glUniform3fv(locs.diffuse_loc, 1, &diffuse_color[0]);
        if (locs.ambient_loc != -1) glUniform3fv(locs.ambient_loc, 1, &ambient_color[0]);
        if (locs.specular_loc != -1) glUniform3fv(locs.specular_loc, 1, &specular_color[0]);
        if (locs.shininess_loc != -1) glUniform1f(locs.shininess_loc, shininess);
        if (locs.alpha_loc != -1) glUniform1f(locs.alpha_loc, alpha);

        glUniform1i(locs.useTextures_loc, use_textures ? 1 : 0);
        glUniform1i(locs.useFlatShading_loc, use_flat ? 1 : 0);
    }

    void unbind() const {
        if (diffuse) diffuse->unbind();
        if (normal) normal->unbind();
        if (roughness) roughness->unbind();
    }
};
