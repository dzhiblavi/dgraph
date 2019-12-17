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

namespace dgl {
class shader {
    GLuint id_ = 0;
    void init_(char const* source_code, GLuint shader_type);

public:
    shader(std::string const& file_path, GLuint shader_type);
    shader(char const* source_code, GLuint shader_type);
    ~shader();

    [[nodiscard]] GLuint native_handle() const noexcept;
};

class gp_prog {
    GLuint id_;

public:
    explicit gp_prog(std::vector<shader> const& shaders);
    ~gp_prog();

    [[nodiscard]] GLuint native_handle() const noexcept;
    [[nodiscard]] GLint uniform_location(std::string const& name);
    void use();
};
}

#endif //DGRAPH_SHADER_H
