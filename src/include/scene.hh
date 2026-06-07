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
#include "maze.hh"
#include "node.hh"

class Scene {
   public:
    Camera camera;
    Lights lights;
    Node root;

   private:
    GLint model_loc;
    GLint vp_loc;
    GLint tr_inv_model_loc;

   public:
    explicit Scene(const Shaders& shaders) : camera(shaders), lights(shaders) {
        camera.view_normal();
        locations(shaders);
        update_all();
    }

    void locations(const Shaders& shaders) {
        camera.locations(shaders);
        lights.locations(shaders);
        model_loc = glGetUniformLocation(shaders.program, "model");
        vp_loc = glGetUniformLocation(shaders.program, "vp");
        tr_inv_model_loc = glGetUniformLocation(shaders.program, "tr_inv_model");

        glUniform1i(glGetUniformLocation(shaders.program, "diffuseMap"), 0);
        glUniform1i(glGetUniformLocation(shaders.program, "normalMap"), 1);
        glUniform1i(glGetUniformLocation(shaders.program, "roughnessMap"), 2);
    }

    void update_all() {
        camera.projection();
        lights.parameters();
        lights.position(camera.inv_v);
    }

    void draw() {
        // clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(vp_loc, 1, GL_FALSE, glm::value_ptr(camera.vp));

        root.draw(model_loc, tr_inv_model_loc, glm::mat4(1.0f));
    }
};

#endif
