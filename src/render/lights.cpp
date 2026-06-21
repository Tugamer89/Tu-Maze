#include "render/lights.hh"

void Lights::locations(const Shaders& shaders) {
    light_direct_pos_loc = glGetUniformLocation(shaders.getProgram(), "light.direct_pos");
    light_direct_val_loc = glGetUniformLocation(shaders.getProgram(), "light.direct_val");
    light_ambient_val_loc = glGetUniformLocation(shaders.getProgram(), "light.ambient_val");
}

void Lights::parameters() const {
    glm::vec3 current_direct_val = is_on ? light_direct_val : glm::vec3(0.0f);
    glUniform3fv(light_direct_val_loc, 1, &current_direct_val[0]);
    glUniform3fv(light_ambient_val_loc, 1, &light_ambient_val[0]);
}

void Lights::position(const glm::mat4& inverse_view_matrix) const {
    glm::vec4 ldp4(light_direct_pos, 1.0f);
    ldp4 = inverse_view_matrix * ldp4;
    glm::vec3 ldp3 = {ldp4.x, ldp4.y, ldp4.z};
    glUniform3fv(light_direct_pos_loc, 1, &ldp3[0]);
}
