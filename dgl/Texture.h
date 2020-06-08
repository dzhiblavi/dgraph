#ifndef DGRAPH_TEXTURE_H
#define DGRAPH_TEXTURE_H

#include <stdexcept>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace dgl {
class texture2d {
    GLuint id_ = 0;

public:
    texture2d(std::filesystem::path const& path, bool sRGB = false);
    ~texture2d();

    void bind();
    [[nodiscard]] GLuint native_handle();
};
}

#endif //DGRAPH_TEXTURE_H