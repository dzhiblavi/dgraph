#ifndef DGRAPH_SHADER_H
#define DGRAPH_SHADER_H

#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "dgl/error.h"

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

private:
    void load_shaders();

    template <typename T, typename... S>
    void load_shaders(T&& s, S&&... ss) {
        glAttachShader(id, s.native_handle());
        load_shaders(std::forward<S>(ss)...);
    }

public:
    template <typename... S>
    GpProg(S&&... ss) {
        id = glCreateProgram();
        load_shaders(std::forward<S>(ss)...);

        glLinkProgram(id);
        glCheckError();
    }

    ~GpProg();

    GpProg& operator=(GpProg const&) = delete;
    GpProg(GpProg const&) = delete;

    GpProg& operator=(GpProg&&);
    GpProg(GpProg&&);

    void swap(GpProg& p);
    void use();
    [[nodiscard]] GLuint native_handle() const noexcept;
    [[nodiscard]] GLint uniform_location(std::string const& name);
};
}


#endif //DGRAPH_SHADER_H
