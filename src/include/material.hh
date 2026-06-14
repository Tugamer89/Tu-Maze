#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <glm/vec3.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "texture.hh"

struct MaterialLocations {
    GLint diffuse_loc = -1;
    GLint ambient_loc = -1;
    GLint specular_loc = -1;
    GLint shininess_loc = -1;
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

    void bind(const MaterialLocations& locs) const {
        if (diffuse) diffuse->bind(0);
        if (normal) normal->bind(1);
        if (roughness) roughness->bind(2);

        // Send material properties to the shader
        if (locs.diffuse_loc != -1) glUniform3fv(locs.diffuse_loc, 1, &diffuse_color[0]);
        if (locs.ambient_loc != -1) glUniform3fv(locs.ambient_loc, 1, &ambient_color[0]);
        if (locs.specular_loc != -1) glUniform3fv(locs.specular_loc, 1, &specular_color[0]);
        if (locs.shininess_loc != -1) glUniform1f(locs.shininess_loc, shininess);
    }
};

#endif
