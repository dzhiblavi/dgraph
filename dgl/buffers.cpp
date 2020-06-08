#include "buffers.h"

namespace dgl {
VBO::VBO() {
    glGenBuffers(1, &id);
}

VBO& VBO::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
    return *this;
}

VBO& VBO::buffer_data(void const* data, size_t size, GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    return *this;
}

VBO::native_handle_t VBO::native_handle() const noexcept {
    return id;
}


VAO::VAO() {
    glGenVertexArrays(1, &id);
}

VAO& VAO::bind() {
    glBindVertexArray(id);
    return *this;
}

VBO& VAO::vbo() noexcept {
    return bo;
}

VBO const& VAO::vbo() const noexcept {
    return bo;
}

VAO& VAO::setup_attribute(Attribute const& attr) {
    glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized
            , attr.stride, attr.offset);
    return *this;
}

VAO& VAO::enable_attribute(int n) {
    glEnableVertexAttribArray(n);
    return *this;
}

VAO::native_handle_t VAO::native_handle() const noexcept {
    return id;
}
} // namespace dgl
