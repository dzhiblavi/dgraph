#ifndef DGRAPH_TEXTURE_H
#define DGRAPH_TEXTURE_H

#include <stdexcept>
#include <filesystem>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace dgl {
class Texture {
public:
    typedef GLuint native_handle_t;

private:
    GLuint id = 0;

public:
    Texture(native_handle_t handle);

    Texture(Texture const&) = delete;

    Texture& operator=(Texture const&) = delete;

    Texture(Texture&&);

    Texture& operator=(Texture&&);

    ~Texture();

    Texture& bind();

    Texture& bind_as(int n);

    [[nodiscard]] native_handle_t native_handle() const noexcept;

    static Texture load_texture2d(std::filesystem::path const& path, bool sRGB = false);

    static Texture load_textureCube2d(std::vector<std::filesystem::path> const& faces);

    static Texture genDepthTex2d(size_t width, size_t height);

    static Texture genDepthTexCube(size_t width, size_t height);
};
}

#endif //DGRAPH_TEXTURE_H
