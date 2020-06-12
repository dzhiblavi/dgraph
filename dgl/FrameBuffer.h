#ifndef DGL_FRAMEBUFFER_H
#define DGL_FRAMEBUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "dgl/error.h"

namespace dgl {
class FrameBuffer {
public:
    typedef GLuint native_handle_t;

private:
    native_handle_t id = 0;

public:
    FrameBuffer();

    FrameBuffer(native_handle_t nh);

    FrameBuffer(FrameBuffer const&) = delete;

    FrameBuffer& operator=(FrameBuffer const&) = delete;

    FrameBuffer(FrameBuffer&&);

    FrameBuffer& operator=(FrameBuffer&&);

    ~FrameBuffer();

    void swap(FrameBuffer& fb);

    void bind(GLenum target);

    static void reset(GLenum target);
};
} // namespace dgl

#endif // DGL_FRAMEBUFFER_H

