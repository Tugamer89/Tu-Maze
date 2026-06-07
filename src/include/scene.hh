#ifndef SCENE_HH
#define SCENE_HH

#include <glm/mat4x4.hpp>
#include <string>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#endif

#include "camera.hh"
#include "gpumesh.hh"
#include "lights.hh"
#include "matrices.hh"

class Scene {
   public:
    Camera camera;
    Lights lights;
    GPUMesh mesh;
    glm::mat4 mesh_mm;

   private:
    GLint model_loc;
    GLint vp_loc;
    GLint tr_inv_model_loc;

   public:
    Scene(const std::string& meshfile, const Shaders& shaders)
        : camera(shaders), lights(shaders), mesh(meshfile) {
        camera.view_normal();
        locations(shaders);
        update_all();
        mesh_mm = scaling(1.0f / mesh.extent) * translation(-mesh.center);
    }

    void locations(const Shaders& shaders) {
        camera.locations(shaders);
        lights.locations(shaders);
        model_loc = glGetUniformLocation(shaders.program, "model");
        vp_loc = glGetUniformLocation(shaders.program, "vp");
        tr_inv_model_loc = glGetUniformLocation(shaders.program, "tr_inv_model");
    }

    void update_all() {
        camera.projection();
        lights.parameters();
        lights.position(camera.inv_v);
    }

    void draw() {
        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 rx90 = rotation_x(90.0f);
        glm::mat4 tr1 = translation(0, -0.9f, 0);
        glm::mat4 full_mm = tr1 * rx90 * mesh_mm;  // full model matrix

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, &full_mm[0][0]);
        glUniformMatrix4fv(vp_loc, 1, GL_FALSE, &camera.vp[0][0]);

        glm::mat3 tr_inv_model = glm::transpose(glm::inverse(glm::mat3(full_mm)));
        glUniformMatrix3fv(tr_inv_model_loc, 1, GL_FALSE, &tr_inv_model[0][0]);

        mesh.draw();
    }
};

#endif
