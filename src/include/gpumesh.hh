#pragma once

#include <bit>
#include <glm/mat4x4.hpp>
#include <string>
#include <vector>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif

#include "mesh.hh"

class GPUMesh {
   private:
    glm::vec3 center = {0.0f, 0.0f, 0.0f};
    float extent = 1.0f;
    GLsizei indexCount = 0;

    GLuint vbo = 0;
    GLuint ebo = 0;
    GLuint vao = 0;
    bool initialized = false;

   public:
    explicit GPUMesh(const std::string& filename) { load(filename); }

    explicit GPUMesh(const Mesh& cpuMesh)
        : center(cpuMesh.getCenter()), extent(cpuMesh.getExtent()), initialized(true) {
        std::vector<float> points;
        std::vector<unsigned int> indices;
        cpuMesh.pack4gpu(points, indices);
        send_arrays_2a3f(points, indices);
    }

    ~GPUMesh() { clean(); }

    GPUMesh(const GPUMesh&) = delete;
    GPUMesh& operator=(const GPUMesh&) = delete;
    GPUMesh(GPUMesh&&) = delete;
    GPUMesh& operator=(GPUMesh&&) = delete;

    [[nodiscard]] const glm::vec3& getCenter() const { return center; }
    [[nodiscard]] float getExtent() const { return extent; }
    [[nodiscard]] GLsizei getIndexCount() const { return indexCount; }

    void load(const std::string& filename) {
        Mesh mesh(filename);
        center = mesh.getCenter();
        extent = mesh.getExtent();

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
        if (!initialized) return;
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }

   private:
    // Sends the interleaved float array (Position: 3, Normal: 3) to the GPU
    void send_arrays_2a3f(const std::vector<float>& points,
                          const std::vector<unsigned int>& indices) {
        indexCount = static_cast<GLsizei>(indices.size());

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Attribute 0: Vertex Position (vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Attribute 1: Vertex Normal (vec3)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              std::bit_cast<void*>(static_cast<std::uintptr_t>(3 * sizeof(float))));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                     GL_STATIC_DRAW);
    }
};
