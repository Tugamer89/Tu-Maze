#pragma once

#include <string>
#include <vector>

#include "glad/gl.h"
#include "render/mesh.hh"

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

    explicit GPUMesh(const Mesh& cpuMesh);

    ~GPUMesh() { clean(); }

    GPUMesh(const GPUMesh&) = delete;
    GPUMesh& operator=(const GPUMesh&) = delete;
    GPUMesh(GPUMesh&&) = delete;
    GPUMesh& operator=(GPUMesh&&) = delete;

    [[nodiscard]] const glm::vec3& getCenter() const { return center; }
    [[nodiscard]] float getExtent() const { return extent; }
    [[nodiscard]] GLsizei getIndexCount() const { return indexCount; }

    void load(const std::string& filename);
    void clean();
    void draw() const;

   private:
    // Sends the interleaved float array (Position: 3, Normal: 3) to the GPU
    void send_arrays_2a3f(const std::vector<float>& points,
                          const std::vector<unsigned int>& indices);
};
