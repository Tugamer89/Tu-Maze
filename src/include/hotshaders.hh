#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef GLAD_GL_IMPLEMENTATION
#define GLAD_GL_IMPLEMENTATION
#include "../glad/gl.h"
#endif
// Returns a C++ string loaded with the contents of a whole file
inline std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + filename);
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
   public:
    GLuint program = 0;

    Shaders() { load_fallback(); }

    Shaders(const std::string& vertex_file, const std::string& fragment_file) {
        try {
            load(vertex_file, fragment_file);
        } catch (const std::exception& e) {
            std::cerr << "WARNING (Shaders): " << e.what()
                      << "\nReverting to static fallback shader.\n";
            load_fallback();
        }
    }

    ~Shaders() { clean(); }

    Shaders(const Shaders&) = delete;
    Shaders& operator=(const Shaders&) = delete;
    Shaders(Shaders&&) = delete;
    Shaders& operator=(Shaders&&) = delete;

    // Hardcoded minimal shader to ensure the engine doesn't crash if external assets are missing
    void load_fallback() {
        const char* vertex_source =
            "#version 410 core\n"
            "layout(location = 0) in vec3 vp;"
            "void main() {"
            "  gl_Position = vec4 (vp, 1.0);"
            "}";
        const char* fragment_source =
            "#version 410 core\n"
            "out vec4 frag_colour;"
            "void main() {"
            "  frag_colour = vec4 (0.8, 0.1, 0.0, 1.0);"
            "}";

        if (!compile_attach_link(&vertex_source, &fragment_source)) {
            throw std::runtime_error("Critical error: Failed to compile fallback shaders.");
        }
    }

    void load(const std::string& vertex_file, const std::string& fragment_file) {
        const std::string vertex_string = read_file(vertex_file);
        const std::string fragment_string = read_file(fragment_file);
        const char* vertex_source = vertex_string.c_str();
        const char* fragment_source = fragment_string.c_str();

        if (!compile_attach_link(&vertex_source, &fragment_source)) {
            throw std::runtime_error("Failed to compile or link provided shader files.");
        }
    }

    void clean() {
        if (program != 0) {
            glDeleteProgram(program);
            program = 0;
        }
    }

    void reload(const std::string& vertex_file, const std::string& fragment_file) {
        clean();
        try {
            load(vertex_file, fragment_file);
        } catch (const std::exception& e) {
            std::cerr << "Shader reload failed: " << e.what() << "\n";
            load_fallback();
        }
    }

    bool compile_attach_link(const char** vertex_source_ptr, const char** fragment_source_ptr) {
        int success = false;

        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, vertex_source_ptr, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::cerr << "Error compiling vertex shader:\n"
                      << getInfoLog(vertex, glGetShaderiv, glGetShaderInfoLog) << std::endl;
            glDeleteShader(vertex);
            return false;
        }

        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, fragment_source_ptr, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            std::cerr << "Error compiling fragment shader:\n"
                      << getInfoLog(fragment, glGetShaderiv, glGetShaderInfoLog) << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return false;
        }

        program = glCreateProgram();
        glAttachShader(program, fragment);
        glAttachShader(program, vertex);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        // Shaders can be safely detached and deleted once compiled into the Program
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        if (!success) {
            std::cerr << "Error linking shaders:\n"
                      << getInfoLog(program, glGetProgramiv, glGetProgramInfoLog) << std::endl;
            glDeleteProgram(program);
            program = 0;
            return false;
        }

        return true;
    }

    void use() const { glUseProgram(program); }
};
