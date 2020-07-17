#include <filesystem>
#include <string>
#include <vector>

#include "buffers.h"
#include "Texture.h"
#include "Shader.h"

namespace dgl {
class SkyBox {
    Texture tex;
    VAO vao;
    Buffer vbo;

public:
    SkyBox(std::filesystem::path const& root, std::string const& ext);

    SkyBox(std::vector<std::filesystem::path> const& files);

    void draw(GpProg& prog);
};
} // namespace dgl
