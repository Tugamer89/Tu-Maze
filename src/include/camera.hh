#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "hotshaders.hh"

class Camera {
   public:
    glm::mat4 v;
    glm::mat4 inv_v;
    glm::mat4 vp;

   private:
    float phi_deg = 0.0;
    float theta_deg = 0.0;

    const float normal_fd = 4.0;

    float fd;  // focal distance
    float od;  // object distance

    GLint camera_pos_loc;                    // xyz
    glm::vec3 camera_pos = {0.0, 0.0, 0.0};  // xyz

   public:
    explicit Camera(const Shaders& shaders) { locations(shaders); }

    void locations(const Shaders& shaders) {
        camera_pos_loc = glGetUniformLocation(shaders.program, "camera_pos");
    }

    void drag(float dx, float dy) {
        phi_deg += dx * 0.1f;
        theta_deg += dy * 0.1f;
        theta_deg = theta_deg > 90.0f ? 90.0f : theta_deg;
        theta_deg = theta_deg < -90.0f ? -90.0f : theta_deg;
        projection();
    }

    void dolly(float dy) {
        float ratio = od / 100.0f;
        od -= dy * ratio;  // note: we go in the opposite direction of zoooming
        if (od < 0.5) od = 0.5;
        projection();
    }

    void view_normal() {
        fd = normal_fd;
        od = normal_fd;
        projection();
    }

    void projection() {
        float ncp = od - 1.0f;  // distance near clip plane
        if (ncp < 0.1) ncp = 0.1f;
        float fcp = od + 1.0f;  // distance far clip plane

        // prepare rotation matrices
        float ps = glm::sin(glm::radians(phi_deg));
        float pc = glm::cos(glm::radians(phi_deg));
        glm::mat4 ry(            //
            pc, 0.0, -ps, 0.0,   // 1st column
            0.0, 1.0, 0.0, 0.0,  // 2nd column
            ps, 0.0, pc, 0.0,    // 3rd column
            0.0, 0.0, 0.0, 1.0   //
        );

        float ts = glm::sin(glm::radians(theta_deg));
        float tc = glm::cos(glm::radians(theta_deg));
        glm::mat4 rx(            //
            1.0, 0.0, 0.0, 0.0,  // 1st column
            0.0, tc, ts, 0.0,    // 2nd column
            0.0, -ts, tc, 0.0,   // 3rd column
            0.0, 0.0, 0.0, 1.0   //
        );

        // prepare translation matrix
        glm::mat4 tz(            //
            1.0, 0.0, 0.0, 0.0,  // 1st column
            0.0, 1.0, 0.0, 0.0,  // 2nd column
            0.0, 0.0, 1.0, 0.0,  // 3rd column
            0.0, 0.0, -od, 1.0   // translate world along the Z axis
        );
        v = tz * rx * ry;

        inv_v = glm::inverse(v);

        // prepare projection matrix
        float a = (fcp + ncp) / (ncp - fcp);
        float b = 2.0f * fcp * ncp / (ncp - fcp);

        glm::mat4 pr(           //
            fd, 0.0, 0.0, 0.0,  // 1st column
            0.0, fd, 0.0, 0.0,  // 2nd column
            0.0, 0.0, a, -1.0,  // 3rd column
            0.0, 0.0, b, 0.0    // 4th column
        );

        // Compute VP matrix and update it
        vp = pr * v;
        inv_v = glm::inverse(v);

        glm::vec4 cp4 = {0.0, 0.0, 0.0, 1.0};
        cp4 = inv_v * cp4;
        glm::vec3 cp3 = {cp4.x, cp4.y, cp4.z};
        glUniform3fv(camera_pos_loc, 1, &cp3[0]);
    }
};

#endif
