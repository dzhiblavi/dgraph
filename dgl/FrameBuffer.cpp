#include "FrameBuffer.h"

namespace dgl {
FrameBuffer::FrameBuffer() {
    glGenFramebuffers(1, &id);
}

FrameBuffer::FrameBuffer(native_handle_t nh) 
    : id(nh) {}

FrameBuffer::FrameBuffer(FrameBuffer&& b) {
    swap(b);
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& fb) {
    swap(fb);
    return *this;
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &id);
}

void FrameBuffer::swap(FrameBuffer& b) {
    std::swap(id, b.id);
}

void FrameBuffer::bind(GLenum target) {
    glBindFramebuffer(target, id);
}

void FrameBuffer::reset(GLenum target) {
    glBindFramebuffer(target, 0);
}
} // namespace dgl
