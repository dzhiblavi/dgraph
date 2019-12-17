#ifndef DGRAPH_TEXTURE_H
#define DGRAPH_TEXTURE_H

#include <string>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace dgl {
class texture2d {
    GLuint id_ = 0;

public:
    texture2d(std::string const& path, bool sRGB = false);
    ~texture2d();

    [[nodiscard]] GLuint native_handle();
    void bind();
};
}

#endif //DGRAPH_TEXTURE_H
