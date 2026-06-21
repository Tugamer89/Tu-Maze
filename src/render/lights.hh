#pragma once

#include <glm/mat4x4.hpp>

#include "glad/gl.h"
#include "render/hotshaders.hh"

class Lights {
   private:
    glm::vec3 light_direct_pos = {0.12f, -0.1f, -0.18f};
    glm::vec3 light_direct_val = {0.85f, 0.55f, 0.25f};
    glm::vec3 light_ambient_val = {0.015f, 0.015f, 0.025f};
    bool is_on = true;

    GLint light_direct_pos_loc = -1;
    GLint light_direct_val_loc = -1;
    GLint light_ambient_val_loc = -1;

   public:
    explicit Lights(const Shaders& shaders) { locations(shaders); }

    [[nodiscard]] const glm::vec3& getDirectPos() const { return light_direct_pos; }
    void setDirectPos(const glm::vec3& pos) { light_direct_pos = pos; }

    [[nodiscard]] const glm::vec3& getDirectVal() const { return light_direct_val; }
    void setDirectVal(const glm::vec3& val) { light_direct_val = val; }

    [[nodiscard]] const glm::vec3& getAmbientVal() const { return light_ambient_val; }
    void setAmbientVal(const glm::vec3& val) { light_ambient_val = val; }

    [[nodiscard]] bool isOn() const { return is_on; }
    void setOn(bool on) { is_on = on; }

    void locations(const Shaders& shaders);

    void parameters() const;

    // Transforms light coordinates relative to camera view space for shader calculations
    void position(const glm::mat4& inverse_view_matrix) const;
};
