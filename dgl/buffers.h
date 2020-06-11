#ifndef DGL_VBO_H
#define DGL_VBO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "error.h"

namespace dgl {
class Buffer {
public:
    typedef GLuint native_handle_t;

protected:
    native_handle_t id = 0;

public:
    Buffer();

    Buffer(native_handle_t id);

    ~Buffer();

    Buffer(Buffer const&) = delete;

    Buffer& operator=(Buffer const&) = delete;

    Buffer(Buffer&& b);

    Buffer& operator=(Buffer&& b);

    void swap(Buffer& b);

    void bind(GLenum target);

    static void unbind(GLenum target);

    static void bufferData(GLenum target, void const* data, size_t size, GLenum usage);

    static void bufferStorage(GLenum target, void const* data, size_t size, GLbitfield flags);

    static void bufferSubData(GLenum target, void const* data, size_t size, GLintptr offset);

    static void clearData(GLenum target, GLenum internalf, GLenum format
            , GLenum type, void const* data);

    static void clearSubData(GLenum target, GLenum internalformat, GLintptr offset
            , GLsizeiptr size, GLenum format, GLenum type, void const* data);

    static void* map(GLenum target, GLbitfield access);

    static void unmap(GLenum target);

    [[nodiscard]] native_handle_t native_handle() const noexcept;

    static void copy(Buffer const& src, Buffer& dst, GLintptr readoffset
        , GLintptr writeoffset, GLsizeiptr size);
};

class VAO {
public:
    typedef GLuint native_handle_t;

private:
    native_handle_t id = 0;

public:
    VAO();

    ~VAO();

    void bind();

    static void unbind();

    static void setupAttribute(GLuint index, GLint size, GLenum type
            , GLboolean norm, GLsizei stride, void const* offset);

    static void enableAttribute(int n);

    static void divisor(GLuint index, GLuint divisor);

    [[nodiscard]] native_handle_t native_handle() const noexcept;
};
} // namespace dgl

#endif // DGL_VBO_H
