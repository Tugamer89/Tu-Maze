#include "render/gpumesh.hh"

#include <bit>

GPUMesh::GPUMesh(const Mesh& cpuMesh)
    : center(cpuMesh.getCenter()), extent(cpuMesh.getExtent()), initialized(true) {
    std::vector<float> points;
    std::vector<unsigned int> indices;
    cpuMesh.pack4gpu(points, indices);
    send_arrays_2a3f(points, indices);
}

void GPUMesh::load(const std::string& filename) {
    Mesh mesh(filename);
    center = mesh.getCenter();
    extent = mesh.getExtent();

    std::vector<float> points;
    std::vector<unsigned int> indices;
    mesh.pack4gpu(points, indices);
    send_arrays_2a3f(points, indices);
    initialized = true;
}

void GPUMesh::clean() {
    if (initialized) {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        initialized = false;
    }
}

void GPUMesh::draw() const {
    if (!initialized) return;
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void GPUMesh::send_arrays_2a3f(const std::vector<float>& points,
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
