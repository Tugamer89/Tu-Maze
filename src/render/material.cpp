#include "render/material.hpp"

void Material::bind(const MaterialLocations& locs) const {
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

void Material::unbind() const {
    if (diffuse) diffuse->unbind();
    if (normal) normal->unbind();
    if (roughness) roughness->unbind();
}
