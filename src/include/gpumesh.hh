#ifndef GPUMESH_HH
#define GPUMESH_HH

#include <glm/mat4x4.hpp>
#include <string>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "mesh.hh"

class GPUMesh {
   public:
    glm::vec3 center = {0.0, 0.0, 0.0};
    float extent = 1.0;
    GLsizei indexCount = 0;

   private:
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint vao = 0;
    bool initialized = false;

   public:
    explicit GPUMesh(const std::string& filename) { load(filename); }

    explicit GPUMesh(Mesh& cpuMesh)
        : center(cpuMesh.center), extent(cpuMesh.extent), initialized(true) {
        std::vector<float> points;
        std::vector<unsigned int> indices;
        cpuMesh.pack4gpu(points, indices);
        send_arrays_2a3f(points, indices);
    }

    ~GPUMesh() { clean(); }

    GPUMesh(const GPUMesh&) = delete;
    GPUMesh& operator=(const GPUMesh&) = delete;

    void load(const std::string& filename) {
        Mesh mesh(filename);
        center = mesh.center;
        extent = mesh.extent;

        std::vector<float> points;
        std::vector<unsigned int> indices;
        mesh.pack4gpu(points, indices);
        send_arrays_2a3f(points, indices);
        initialized = true;
    }

    void clean() {
        if (initialized) {
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            initialized = false;
        }
    }

    void draw() const {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }

   private:
    void send_arrays_2a3f(const std::vector<float>& points,
                          const std::vector<unsigned int>& indices) {
        indexCount = static_cast<GLsizei>(indices.size());

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    }
};

#endif
