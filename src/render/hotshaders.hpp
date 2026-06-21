#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include "core/exceptions.hpp"
#include "glad/gl.h"

inline std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw exceptions::ShaderException("Failed to open shader file: " + filename);
    }

    std::string s;
    s.reserve(std::filesystem::file_size(filename));
    s.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return s;
}

// Interrogates OpenGL to extract compilation/linking error logs
template <typename GetIvFunc, typename GetInfoLogFunc>
inline std::string getInfoLog(GLuint object, GetIvFunc get_iv, GetInfoLogFunc get_infolog) {
    GLint loglen = 0;
    get_iv(object, GL_INFO_LOG_LENGTH, &loglen);

    if (loglen == 0) return {};

    std::string info_log;
    info_log.resize(loglen);
    get_infolog(object, loglen, nullptr, info_log.data());

    if (!info_log.empty() && info_log.back() == '\0') {
        info_log.pop_back();
    }

    return info_log;
}

// Manages the compilation, linking, and hot-reloading of GLSL Shader Programs
class Shaders {
   private:
    GLuint program = 0;

   public:
    Shaders() { load_fallback(); }

    Shaders(const std::string& vertex_file, const std::string& fragment_file);

    ~Shaders() { clean(); }

    Shaders(const Shaders&) = delete;
    Shaders& operator=(const Shaders&) = delete;
    Shaders(Shaders&&) = delete;
    Shaders& operator=(Shaders&&) = delete;

    [[nodiscard]] GLuint getProgram() const { return program; }

    // Hardcoded minimal shader to ensure the engine doesn't crash if external assets are missing
    void load_fallback();

    void load(const std::string& vertex_file, const std::string& fragment_file);

    void clean();

    void reload(const std::string& vertex_file, const std::string& fragment_file);

    bool compile_attach_link(const char** vertex_source_ptr, const char** fragment_source_ptr);

    void use() const { glUseProgram(program); }
};
