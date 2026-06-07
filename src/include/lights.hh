#ifndef LIGHTS_HH
#define LIGHTS_HH

#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "hotshaders.hh"

class Lights {
   public:
    glm::vec3 light_direct_pos = {2.0, 2.0, 0.0};   // xyz
    glm::vec3 light_direct_val = {1.0, 1.0, 1.0};   // rgb
    glm::vec3 light_ambient_val = {0.1, 0.1, 0.1};  // rgb
    glm::vec3 material_diffuse = {0.8, 0.7, 0.6};   // rgb
    glm::vec3 material_ambient = {0.5, 0.5, 0.8};   // rgb
    glm::vec3 material_specular = {1.0, 1.0, 1.0};  // rgb
    float material_shininess = 1000.f;              // scalar

   private:
    // lights and materials
    GLint light_direct_pos_loc;    // xyz
    GLint light_direct_val_loc;    // rgb
    GLint light_ambient_val_loc;   // rgb
    GLint material_diffuse_loc;    // rgb
    GLint material_ambient_loc;    // rgb
    GLint material_specular_loc;   // rgb
    GLint material_shininess_loc;  // scalar

   public:
    explicit Lights(const Shaders& shaders) { locations(shaders); }

    void locations(const Shaders& shaders) {
        light_direct_pos_loc = glGetUniformLocation(shaders.program, "light.direct_pos");
        light_direct_val_loc = glGetUniformLocation(shaders.program, "light.direct_val");
        light_ambient_val_loc = glGetUniformLocation(shaders.program, "light.ambient_val");
        material_diffuse_loc = glGetUniformLocation(shaders.program, "material.diffuse");
        material_ambient_loc = glGetUniformLocation(shaders.program, "material.ambient");
        material_specular_loc = glGetUniformLocation(shaders.program, "material.specular");
        material_shininess_loc = glGetUniformLocation(shaders.program, "material.shininess");
    }

    void parameters() {
        glUniform3fv(light_direct_val_loc, 1, &light_direct_val[0]);
        glUniform3fv(light_ambient_val_loc, 1, &light_ambient_val[0]);
        glUniform3fv(material_diffuse_loc, 1, &material_diffuse[0]);
        glUniform3fv(material_ambient_loc, 1, &material_ambient[0]);
        glUniform3fv(material_specular_loc, 1, &material_specular[0]);
        glUniform1fv(material_shininess_loc, 1, &material_shininess);
    }

    void position(const glm::mat4& inverse_view_matrix) const {
        glm::vec4 ldp4(light_direct_pos, 1.0);
        ldp4 = inverse_view_matrix * ldp4;
        glm::vec3 ldp3 = {ldp4.x, ldp4.y, ldp4.z};
        glUniform3fv(light_direct_pos_loc, 1, &ldp3[0]);
    }
};

#endif
