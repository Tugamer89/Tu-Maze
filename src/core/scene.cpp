#include "core/scene.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <string>

#include "game/maze.hpp"

Scene::Scene(const Shaders& shaders) : camera(shaders), lights(shaders) {
    locations(shaders);
    update_all();
}

void Scene::locations(const Shaders& shaders) {
    camera.locations(shaders);
    lights.locations(shaders);

    model_loc = glGetUniformLocation(shaders.getProgram(), "model");
    vp_loc = glGetUniformLocation(shaders.getProgram(), "vp");
    tr_inv_model_loc = glGetUniformLocation(shaders.getProgram(), "tr_inv_model");

    mat_locs.diffuse_loc = glGetUniformLocation(shaders.getProgram(), "material.diffuse");
    mat_locs.ambient_loc = glGetUniformLocation(shaders.getProgram(), "material.ambient");
    mat_locs.specular_loc = glGetUniformLocation(shaders.getProgram(), "material.specular");
    mat_locs.shininess_loc = glGetUniformLocation(shaders.getProgram(), "material.shininess");
    mat_locs.alpha_loc = glGetUniformLocation(shaders.getProgram(), "material.alpha");
    mat_locs.useTextures_loc = glGetUniformLocation(shaders.getProgram(), "useTextures");
    mat_locs.useFlatShading_loc = glGetUniformLocation(shaders.getProgram(), "useFlatShading");

    glUniform1i(glGetUniformLocation(shaders.getProgram(), "diffuseMap"), 0);
    glUniform1i(glGetUniformLocation(shaders.getProgram(), "normalMap"), 1);
    glUniform1i(glGetUniformLocation(shaders.getProgram(), "roughnessMap"), 2);
}

void Scene::update_all() {
    camera.projection();
    lights.parameters();
    lights.position(camera.getInverseViewMatrix());
}

void Scene::build_static_tree() {
    root.updateTransforms();
    goalNode.updateTransforms();
}

void Scene::update_gameplay(sf::Time dt, const glm::vec3& goalWorldPos) {
    static float bobAngle = 0.0f;
    static float rotAngle = 0.0f;
    const float TWO_PI = 2.0f * glm::pi<float>();

    bobAngle += dt.asSeconds() * 3.0f;
    rotAngle += dt.asSeconds() * 2.0f;

    if (bobAngle > TWO_PI) bobAngle -= TWO_PI;
    if (rotAngle > TWO_PI) rotAngle -= TWO_PI;

    glm::mat4 goalTransform = glm::translate(
        glm::mat4(1.0f), goalWorldPos + glm::vec3(0.0f, std::sin(bobAngle) * 0.1f, 0.0f));
    goalTransform = glm::rotate(goalTransform, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    goalTransform = glm::scale(goalTransform, glm::vec3(0.4f));

    goalNode.setLocalMatrix(goalTransform);
    goalNode.updateTransforms();
}

bool Scene::check_win_condition(const glm::vec3& goalPos) const {
    float distToGoal = glm::distance(camera.getPosition(), goalPos);
    return distToGoal <= Maze::CELL_SIZE * 0.5f;
}

void Scene::draw() const {
    glUniformMatrix4fv(vp_loc, 1, GL_FALSE, glm::value_ptr(camera.getViewProjectionMatrix()));

    const Material* currentMat = nullptr;

    root.draw(model_loc, tr_inv_model_loc, camera.getFrustumPlanes(), mat_locs, currentMat);
    goalNode.draw(model_loc, tr_inv_model_loc, camera.getFrustumPlanes(), mat_locs, currentMat);

    if (currentMat) {
        currentMat->unbind();
    }
}
