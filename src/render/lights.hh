#pragma once

#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

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

    void locations(const Shaders& shaders) {
        light_direct_pos_loc = glGetUniformLocation(shaders.getProgram(), "light.direct_pos");
        light_direct_val_loc = glGetUniformLocation(shaders.getProgram(), "light.direct_val");
        light_ambient_val_loc = glGetUniformLocation(shaders.getProgram(), "light.ambient_val");
    }

    void parameters() const {
        glm::vec3 current_direct_val = is_on ? light_direct_val : glm::vec3(0.0f);
        glUniform3fv(light_direct_val_loc, 1, &current_direct_val[0]);
        glUniform3fv(light_ambient_val_loc, 1, &light_ambient_val[0]);
    }

    // Transforms light coordinates relative to camera view space for shader calculations
    void position(const glm::mat4& inverse_view_matrix) const {
        glm::vec4 ldp4(light_direct_pos, 1.0f);
        ldp4 = inverse_view_matrix * ldp4;
        glm::vec3 ldp3 = {ldp4.x, ldp4.y, ldp4.z};
        glUniform3fv(light_direct_pos_loc, 1, &ldp3[0]);
    }
};
