#ifndef LIGHTS_HH
#define LIGHTS_HH

#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "hotshaders.hh"

class Lights {
   public:
    // Placed slightly to the right, down, and forward (like a held torch)
    glm::vec3 light_direct_pos = {0.25f, -0.15f, -0.3f};

    // Warm, orange/yellow fire light
    glm::vec3 light_direct_val = {0.85f, 0.55f, 0.25f};

    // Very dim, cool blueish ambient for a dark dungeon
    glm::vec3 light_ambient_val = {0.015f, 0.015f, 0.025f};

   private:
    GLint light_direct_pos_loc;
    GLint light_direct_val_loc;
    GLint light_ambient_val_loc;

   public:
    explicit Lights(const Shaders& shaders) { locations(shaders); }

    void locations(const Shaders& shaders) {
        light_direct_pos_loc = glGetUniformLocation(shaders.program, "light.direct_pos");
        light_direct_val_loc = glGetUniformLocation(shaders.program, "light.direct_val");
        light_ambient_val_loc = glGetUniformLocation(shaders.program, "light.ambient_val");
    }

    void parameters() {
        glUniform3fv(light_direct_val_loc, 1, &light_direct_val[0]);
        glUniform3fv(light_ambient_val_loc, 1, &light_ambient_val[0]);
    }

    void position(const glm::mat4& inverse_view_matrix) const {
        // Transform the light position relative to the camera into world space
        glm::vec4 ldp4(light_direct_pos, 1.0);
        ldp4 = inverse_view_matrix * ldp4;
        glm::vec3 ldp3 = {ldp4.x, ldp4.y, ldp4.z};
        glUniform3fv(light_direct_pos_loc, 1, &ldp3[0]);
    }
};

#endif
