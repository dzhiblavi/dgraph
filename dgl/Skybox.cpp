#include "Skybox.h"

namespace dgl {
SkyBox::SkyBox(std::filesystem::path const& root, std::string const& ext)
    : SkyBox(std::vector<std::filesystem::path>{
                root / ("right" + ext),
                root / ("left" + ext),
                root / ("top" + ext),
                root / ("bottom" + ext),
                root / ("front" + ext),
                root / ("back" + ext)}) {
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    vbo.bufferData(GL_ARRAY_BUFFER, skyboxVertices, sizeof(skyboxVertices), GL_STATIC_DRAW);
    vao.setupAttribute(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), nullptr);
    vao.enableAttribute(0);
    Buffer::unbind(GL_ARRAY_BUFFER);
    VAO::unbind();
}

SkyBox::SkyBox(std::vector<std::filesystem::path> const& faces) 
    : tex(Texture::load_textureCube2d(faces)) {}

void SkyBox::draw(GpProg& prog) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    vao.bind();
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex.native_handle());
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
} // namespace dgl
