#include "Shader.h"
#include <iostream>

namespace {
GLuint init(char const* source_code, GLuint shader_type) {
    GLuint id = glCreateShader(shader_type);
    glShaderSource(id, 1, &source_code, nullptr);
    glCompileShader(id);

    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        DGL_EXC("shader creation failed: " + std::string(infoLog));
    }

    dgl::glCheckError();
    return id;
}
} // namespace

namespace dgl {
Shader::Shader(std::string const &file_path, GLuint shader_type) {
    std::ifstream t(file_path);
    t.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string sc = buffer.str();
    id = init(sc.data(), shader_type);
}

Shader::Shader(char const *source_code, GLuint shader_type) {
    id = init(source_code, shader_type);
}

Shader::~Shader() {
    glDeleteShader(id);
}

Shader& Shader::operator=(Shader&& s) {
    if (this != &s) {
        swap(s);
    }
    return *this;
}

Shader::Shader(Shader&& s) {
    swap(s);
}

void Shader::swap(Shader& s) {
    std::swap(id, s.id);
}

GLuint Shader::native_handle() const noexcept {
    return id;
}

void GpProg::load_shaders() {}

GpProg::~GpProg() {
    glDeleteProgram(id);
}

GpProg& GpProg::operator=(GpProg&& s) {
    if (this != &s) {
        swap(s);
    }
    return *this;
}

GpProg::GpProg(GpProg&& s) {
    swap(s);
}

void GpProg::swap(GpProg& p) {
    std::swap(id, p.id);
}

GLuint GpProg::native_handle() const noexcept {
    return id;
}

GLint GpProg::uniform_location(std::string const& name) {
    GLint r = glGetUniformLocation(id, name.c_str());
    glCheckError();
    return r;
}

GpProg& GpProg::use() {
    glUseProgram(id);
    return *this;
}
} // namespace dgl
