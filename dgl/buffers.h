#ifndef DGL_VBO_H
#define DGL_VBO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace dgl {
class VBO {
public:
    typedef GLuint native_handle_t;

private:
    GLuint id = 0;

public:
    VBO();

    VBO& bind();

    VBO& buffer_data(void const* data, size_t size, GLenum usage);

    [[nodiscard]] native_handle_t native_handle() const noexcept;
};


class VAO {
public:
    typedef GLuint native_handle_t;

private:
    GLuint id = 0;

public:
    struct Attribute {
        GLuint index = 0;
        GLint size = 0;
        GLenum type = GL_FLOAT;
        GLboolean normalized = GL_FALSE;
        GLsizei stride = 0;
        const void* offset = nullptr;
    };

public:
    VAO();

    VAO& bind();

    VAO& setup_attribute(Attribute const& attr);

    VAO& enable_attribute(int n);

    [[nodiscard]] native_handle_t native_handle() const noexcept;
};


class EBO {
public:
    typedef GLuint native_handle_t;

private:
    GLuint id = 0;

public:
    EBO();

    EBO& bind();

    EBO& buffer_data(void const* data, size_t size, GLenum usage);

    [[nodiscard]] native_handle_t native_handle() const noexcept;
};
} // namespace dgl

#endif // DGL_VBO_H
