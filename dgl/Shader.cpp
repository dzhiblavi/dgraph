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
    std::swap(u_vars, p.u_vars);
}

GLuint GpProg::native_handle() const noexcept {
    return id;
}

GLint GpProg::uniform_location(std::string const& name) {
    auto it = u_vars.find(name);

    if (it != u_vars.end()) {
        return it->second;
    } else {
        GLuint r = (u_vars[name] = glGetUniformLocation(id, name.c_str()));
        glCheckError();
        return r;
    }
}

GpProg& GpProg::use() {
    glUseProgram(id);
    return *this;
}

GpProg::ass_helper GpProg::get() noexcept {
    return ass_helper(*this, "");
}

GpProg::ass_helper GpProg::operator[](std::string const& name) noexcept {
    return ass_helper(*this, name);
}


GpProg::ass_helper::ass_helper(GpProg& prog, std::string const& name)
    : prog(prog), name(name) {}

GLuint GpProg::ass_helper::id() {
    return prog.uniform_location(name);
}

GpProg::ass_helper GpProg::ass_helper::get(size_t ind) {
    return ass_helper(prog, name + "[" + std::to_string(ind) + "]");
}

GpProg::ass_helper GpProg::ass_helper::get(std::string const& field) {
    return ass_helper(prog, name + "." + field);
}

GpProg::ass_helper GpProg::ass_helper::operator[](size_t i) {
    return get(i);
}

GpProg::ass_helper GpProg::ass_helper::operator[](std::string const& field) {
    return get(field);
}

#define DGL_ASS_DCL(type, suff, ...) \
GpProg::ass_helper& GpProg::ass_helper::operator=(type x) { \
    glUniform ## suff (id(), __VA_ARGS__); \
    glCheckError(); \
    return *this; \
}

DGL_ASS_DCL(float, 1f, x)
DGL_ASS_DCL(int, 1i, x)
DGL_ASS_DCL(uint, 1f, x)

DGL_ASS_DCL(glm::vec2 const&, 2fv, 1, glm::value_ptr(x))
DGL_ASS_DCL(glm::vec3 const&, 3fv, 1, glm::value_ptr(x))

DGL_ASS_DCL(glm::mat3x3 const&, Matrix3fv, 1, GL_FALSE, glm::value_ptr(x))
DGL_ASS_DCL(glm::mat4x4 const&, Matrix4fv, 1, GL_FALSE, glm::value_ptr(x))
} // namespace dgl
