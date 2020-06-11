#ifndef DGL_MODEL_H
#define DGL_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"
#include "buffers.h"
#include "Shader.h"

namespace dgl {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

enum TextureType {
    DIFFUSE,
    SPECULAR,
};

struct ModelTexture {
    Texture t;
    TextureType type;

public:
    ModelTexture(Texture&& t, TextureType type);
};

class Mesh {
    std::vector<Vertex> vs;
    std::vector<uint> is;
    std::vector<std::shared_ptr<ModelTexture>> ts;

    VAO vao;
    Buffer vbo;
    Buffer ebo;

public:
    Mesh(std::vector<Vertex>&& vs, std::vector<uint>&& is
            , std::vector<std::shared_ptr<ModelTexture>>&& ts);

    Mesh(Mesh const&) = delete;

    Mesh& operator=(Mesh const&) = delete;

    Mesh(Mesh&&);

    Mesh& operator=(Mesh&&);

    void swap(Mesh& m);

    void draw(GpProg& prog);

    void bindVao();

    std::vector<uint> const& indices() const noexcept;
};

class Model {
    std::vector<Mesh> ms;
    std::filesystem::path dir;
    std::unordered_map<std::string, std::shared_ptr<ModelTexture>> tx_cache;

private:
    void process_node(aiNode* node, aiScene const* scene);

    Mesh process_mesh(aiMesh* mesh, aiScene const* scene);

    std::vector<std::shared_ptr<ModelTexture>> load_material_textures(aiMaterial *mat
            , aiTextureType type, TextureType ttype);

public:
    Model(std::filesystem::path const& p);

    void draw(GpProg& prog);

    std::vector<Mesh>& meshes() noexcept;

    std::vector<Mesh> const& meshes() const noexcept;
};
} // namespace dgl

#endif // DGL_MODEL_H
