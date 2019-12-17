#include "../headers/shader.h"

namespace dgl {
void shader::init_(char const* source_code, GLuint shader_type) {
    id_ = glCreateShader(shader_type);
    glShaderSource(id_, 1, &source_code, nullptr);
    glCompileShader(id_);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(id_, 512, nullptr, infoLog);
        throw std::runtime_error("shader creation failed: " + std::string(infoLog));
    }
}

shader::shader(std::string const &file_path, GLuint shader_type) {
    std::ifstream t(file_path);
    if (t.fail())
        throw std::runtime_error("failed to open shader source file: " + file_path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string sc = buffer.str();
    init_(sc.data(), shader_type);
}

shader::shader(char const *source_code, GLuint shader_type) {
    init_(source_code, shader_type);
}

shader::~shader() {
    glDeleteShader(id_);
}

GLuint shader::native_handle() const noexcept {
    return id_;
}

gp_prog::gp_prog(const std::vector<shader> &shaders)
    : id_(glCreateProgram())
{
    for (auto const& shader : shaders)
        glAttachShader(id_, shader.native_handle());
    glLinkProgram(id_);

    int success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success)
        throw std::runtime_error("failed to link program");
}

gp_prog::~gp_prog() {
    glDeleteProgram(id_);
}

GLuint gp_prog::native_handle() const noexcept {
    return id_;
}

GLint gp_prog::uniform_location(std::string const& name) {
    GLint r = glGetUniformLocation(id_, name.c_str());
    if (r == -1)
        throw std::runtime_error("uniform does not exist");
    return r;
}

void gp_prog::use() {
    glUseProgram(id_);
}
}