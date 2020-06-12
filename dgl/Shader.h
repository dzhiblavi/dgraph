#ifndef DGRAPH_SHADER_H
#define DGRAPH_SHADER_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "error.h"

namespace dgl {
class Shader {
    GLuint id = 0;

public:
    Shader(std::string const& file_path, GLuint shader_type);

    Shader(char const* source_code, GLuint shader_type);

    ~Shader();

    Shader& operator=(Shader const&) = delete;

    Shader(Shader const&) = delete;

    Shader& operator=(Shader&&);

    Shader(Shader&&);

    void swap(Shader& s);

    [[nodiscard]] GLuint native_handle() const noexcept;
};

class GpProg {
    GLuint id;
    std::unordered_map<std::string, GLuint> u_vars;

private:
    void load_shaders();

    template <typename T, typename... S>
    void load_shaders(T&& s, S&&... ss) {
        glAttachShader(id, s.native_handle());
        glCheckError();
        load_shaders(std::forward<S>(ss)...);
    }

public:
    struct ass_helper;

public:
    template <typename... S>
    GpProg(S&&... ss) {
        id = glCreateProgram();
        load_shaders(std::forward<S>(ss)...);

        glLinkProgram(id);

        GLint success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);

        if (!success) {
            GLchar infoLog[512];
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            DGL_EXC("program linkage failed: " + std::string(infoLog));
        }

        glCheckError();
    }

    ~GpProg();

    GpProg& operator=(GpProg const&) = delete;

    GpProg(GpProg const&) = delete;

    GpProg& operator=(GpProg&&);

    GpProg(GpProg&&);

    void swap(GpProg& p);

    GpProg& use();

    [[nodiscard]] GLuint native_handle() const noexcept;

    [[nodiscard]] GLint uniform_location(std::string const& name);

    [[nodiscard]] ass_helper get() noexcept;

    [[nodiscard]] ass_helper operator[](std::string const& name) noexcept;
};

struct GpProg::ass_helper {
private:
    GpProg& prog;
    std::string name;

private:
    GLuint id();

public:
    ass_helper(GpProg& prog, std::string const& name);

    ass_helper get(size_t ind);
    ass_helper get(std::string const& field);
    ass_helper operator[](size_t ind);
    ass_helper operator[](std::string const& field);

    ass_helper& operator=(float x);
    ass_helper& operator=(int x);
    ass_helper& operator=(uint x);
    
    ass_helper& operator=(glm::vec2 const& x);
    ass_helper& operator=(glm::vec3 const& x);

    ass_helper& operator=(glm::mat3x3 const& x);
    ass_helper& operator=(glm::mat4x4 const& x);
};
}


#endif //DGRAPH_SHADER_H
