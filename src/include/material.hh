#ifndef MATERIAL_HH
#define MATERIAL_HH

#include "texture.hh"

struct Material {
    Texture* diffuse = nullptr;
    Texture* normal = nullptr;
    Texture* roughness = nullptr;

    void bind() const {
        if (diffuse) diffuse->bind(0);
        if (normal) normal->bind(1);
        if (roughness) roughness->bind(2);
    }
};

#endif
