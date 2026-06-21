#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

// Handles CPU-side 3D Geometry loading (Object File Format .off) and manipulation
class Mesh {
   private:
    glm::vec3 center = {0.0f, 0.0f, 0.0f};
    float extent = 1.0f;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::uvec3> triangles;

   public:
    Mesh() = default;

    explicit Mesh(const std::string& filename);

    [[nodiscard]] const glm::vec3& getCenter() const { return center; }
    [[nodiscard]] float getExtent() const { return extent; }

    // Merges another mesh into this one while baking in a transformation matrix.
    // Highly efficient for creating batched geometry (like an entire maze level) as one draw call.
    void appendTransformed(const Mesh& source, const glm::mat4& transform);

    // Flattens the structured arrays into interleaved memory ready for OpenGL VBOs
    void pack4gpu(std::vector<float>& points, std::vector<unsigned int>& indices) const;

    void compute_normals();

    // Computes the Axis-Aligned Bounding Box (AABB) and maximum radius for Frustum Culling
    void compute_scale();

    void rescale();
};
