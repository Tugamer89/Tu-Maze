#include "render/mesh.hpp"

#include <fstream>
#include <sstream>

#include "core/exceptions.hpp"

Mesh::Mesh(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw exceptions::MeshException("Failed to open OFF file: " + filename);
    }

    std::string line;

    // Read and validate OFF header
    std::getline(file, line);
    std::erase(line, '\r');
    std::erase(line, '\n');
    if (line != "OFF") {
        throw exceptions::MeshException("Invalid OFF file: missing OFF header in " + filename);
    }

    // Safely skip any leading comments and empty lines
    while (std::getline(file, line)) {
        std::erase(line, '\r');
        std::erase(line, '\n');
        if (line.empty() || line[0] == '#') continue;
        break;
    }

    std::istringstream headerStream(line);
    unsigned int vnum = 0;
    unsigned int fnum = 0;
    unsigned int ednum = 0;
    if (!(headerStream >> vnum >> fnum >> ednum)) {  // NOSONAR
        throw exceptions::MeshException("Invalid OFF header format in " + filename);
    }

    vertices.reserve(vnum);
    normals.reserve(vnum);
    triangles.reserve(fnum);

    for (unsigned int i = 0; i < vnum; ++i) {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
        if (!(file >> x >> y >> z)) {
            throw exceptions::MeshException("Failed to read vertex data in " + filename);
        }
        vertices.emplace_back(x, y, z);
        normals.emplace_back(0.0f, 0.0f, 0.0f);
    }

    for (unsigned int i = 0; i < fnum; ++i) {
        unsigned int vcount = 0;
        if (!(file >> vcount)) {
            throw exceptions::MeshException("Failed to read face count in " + filename);
        }

        if (vcount == 3) {
            glm::uvec3 triangle;
            if (!(file >> triangle[0] >> triangle[1] >> triangle[2])) {
                throw exceptions::MeshException("Failed to read triangle indices in " + filename);
            }
            triangles.push_back(triangle);
        } else {
            throw exceptions::MeshException("Mesh contains non-triangle faces. Unsupported.");
        }
    }

    file.close();

    compute_scale();
    compute_normals();
}

void Mesh::appendTransformed(const Mesh& source, const glm::mat4& transform) {
    auto indexOffset = static_cast<unsigned int>(this->vertices.size());
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

    this->vertices.reserve(this->vertices.size() + source.vertices.size());
    this->normals.reserve(this->normals.size() + source.normals.size());
    this->triangles.reserve(this->triangles.size() + source.triangles.size());

    for (const auto& v : source.vertices) {
        glm::vec4 worldPos = transform * glm::vec4(v, 1.0f);
        this->vertices.push_back(glm::vec3(worldPos));
    }

    for (const auto& n : source.normals) {
        glm::vec3 worldNormal = normalMatrix * n;
        this->normals.push_back(glm::normalize(worldNormal));
    }

    for (const auto& t : source.triangles) {
        this->triangles.push_back(
            glm::uvec3(t[0] + indexOffset, t[1] + indexOffset, t[2] + indexOffset));
    }
}

void Mesh::pack4gpu(std::vector<float>& points, std::vector<unsigned int>& indices) const {
    points.clear();
    points.reserve(vertices.size() * 6);

    for (size_t i = 0; i < vertices.size(); i++) {
        const auto& v = vertices[i];
        const auto& n = normals[i];
        points.push_back(v.x);
        points.push_back(v.y);
        points.push_back(v.z);
        points.push_back(n.x);
        points.push_back(n.y);
        points.push_back(n.z);
    }

    indices.clear();
    indices.reserve(triangles.size() * 3);

    for (const auto& t : triangles) {
        indices.push_back(t[0]);
        indices.push_back(t[1]);
        indices.push_back(t[2]);
    }
}

void Mesh::compute_normals() {
    for (auto t : triangles) {
        glm::vec3 v0 = vertices[t[0]];
        glm::vec3 v1 = vertices[t[1]];
        glm::vec3 v2 = vertices[t[2]];

        glm::vec3 n = glm::cross(v1 - v0, v2 - v0);

        normals[t[0]] += n;
        normals[t[1]] += n;
        normals[t[2]] += n;
    }

    for (auto& n : normals) {
        // Avoid normalizing zero-length vectors
        if (glm::length(n) > 0.0001f) {
            n = glm::normalize(n);
        }
    }
}

void Mesh::compute_scale() {
    if (vertices.empty()) return;

    glm::vec3 min_bounds = vertices[0];
    glm::vec3 max_bounds = vertices[0];

    for (const auto& vertex : vertices) {
        min_bounds = glm::min(min_bounds, vertex);
        max_bounds = glm::max(max_bounds, vertex);
    }

    center = (min_bounds + max_bounds) * 0.5f;
    extent = glm::distance(max_bounds, min_bounds) * 0.5f;
}

void Mesh::rescale() {
    for (auto& vertex : vertices) {
        vertex = (vertex - center) / extent;
    }
    compute_scale();
}
