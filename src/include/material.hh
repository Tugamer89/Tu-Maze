#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <glm/vec3.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#include <cstring>
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "texture.hh"

struct MaterialLocations {
    GLint diffuse_loc = -1;
    GLint ambient_loc = -1;
    GLint specular_loc = -1;
    GLint shininess_loc = -1;
    GLint alpha_loc = -1;
    GLint useTextures_loc = -1;
    GLint useFlatShading_loc = -1;
};

struct Material {
    Texture* diffuse = nullptr;
    Texture* normal = nullptr;
    Texture* roughness = nullptr;

    // Physical Material Properties
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

        // Send material properties to the shader
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

    // Fast memory comparison for caching
    bool operator==(const Material& other) const {
        return diffuse == other.diffuse && normal == other.normal && roughness == other.roughness &&
               diffuse_color[0] == other.diffuse_color[0] &&
               diffuse_color[1] == other.diffuse_color[1] &&
               diffuse_color[2] == other.diffuse_color[2] &&
               ambient_color[0] == other.ambient_color[0] &&
               ambient_color[1] == other.ambient_color[1] &&
               ambient_color[2] == other.ambient_color[2] &&
               specular_color[0] == other.specular_color[0] &&
               specular_color[1] == other.specular_color[1] &&
               specular_color[2] == other.specular_color[2] && shininess == other.shininess &&
               alpha == other.alpha && use_textures == other.use_textures &&
               use_flat == other.use_flat;
    }

    bool operator!=(const Material& other) const { return !(*this == other); }
};

#endif
