#include "buffers.h"

namespace dgl {
Buffer::Buffer() {
    glGenBuffers(1, &id);
}

Buffer::Buffer(native_handle_t id)
    : id(id) {}

Buffer::~Buffer() {
    glDeleteBuffers(1, &id);
}

Buffer::Buffer(Buffer&& b) {
    swap(b);
}

Buffer& Buffer::operator=(Buffer&& b) {
    swap(b);
    return *this;
}

void Buffer::swap(Buffer& b) {
    std::swap(id, b.id);
}

void Buffer::bind(GLenum target) {
    glBindBuffer(target, id);
}

void Buffer::unbind(GLenum target) {
    glBindBuffer(target, 0);
}

void Buffer::bufferData(GLenum target, void const* data, size_t size, GLenum usage) {
    glBufferData(target, size, data, usage);
}

void Buffer::bufferStorage(GLenum target, void const* data, size_t size
        , GLbitfield flags) {
    glBufferStorage(target, size, data, flags);
}

void Buffer::bufferSubData(GLenum target, void const* data, size_t size
        , GLintptr offset) {
    glBufferSubData(target, offset, size, data);
}

void Buffer::clearData(GLenum target, GLenum internalf, GLenum format
    , GLenum type, void const* data) {
    glClearBufferData(target, internalf, format, type, data);
}

void Buffer::clearSubData(GLenum target, GLenum internalf, GLintptr offset
        , GLsizeiptr size, GLenum format, GLenum type, void const* data) {
    glClearBufferSubData(target, internalf, offset, size, format, type, data);
}

void* Buffer::map(GLenum target, GLbitfield access) {
    return glMapBuffer(target, access);
}

void Buffer::unmap(GLenum target) {
    glUnmapBuffer(target); 
}


VAO::VAO() {
    glGenVertexArrays(1, &id);
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &id);
}

void VAO::bind() {
    glBindVertexArray(id);
}

void VAO::unbind() {
    glBindVertexArray(0);
}

void VAO::setupAttribute(GLuint index, GLint size, GLenum type
            , GLboolean norm, GLsizei stride, void const* offset) {
    glVertexAttribPointer(index, size, type, norm, stride, offset);
}

void VAO::enableAttribute(int n) {
    glEnableVertexAttribArray(n);
}

VAO::native_handle_t VAO::native_handle() const noexcept {
    return id;
}
} // namespace dgl

