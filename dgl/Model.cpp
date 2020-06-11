#include "Model.h"

namespace dgl {
ModelTexture::ModelTexture(Texture&& t, TextureType type)
    : t(std::move(t)), type(type) {}

Mesh::Mesh(std::vector<Vertex>&& vs_, std::vector<uint>&& is_
        , std::vector<std::shared_ptr<ModelTexture>>&& ts_)
    : vs(std::move(vs_)), is(std::move(is_)), ts(std::move(ts_)) {
    vao.bind();

    vbo.bind(GL_ARRAY_BUFFER);
    Buffer::bufferData(GL_ARRAY_BUFFER, &vs[0], sizeof(Vertex) * vs.size()
            , GL_STATIC_DRAW);

    ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    Buffer::bufferData(GL_ELEMENT_ARRAY_BUFFER, &is[0], sizeof(uint) * is.size()
            , GL_STATIC_DRAW);

    vao.setupAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    vao.setupAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex)
            , (void*)offsetof(Vertex, normal));
    vao.setupAttribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex)
            , (void*)offsetof(Vertex, texCoords));
    vao.enableAttribute(0);
    vao.enableAttribute(1);
    vao.enableAttribute(2);

    vao.unbind();
    Buffer::unbind(GL_ARRAY_BUFFER);
    Buffer::unbind(GL_ELEMENT_ARRAY_BUFFER);
}

Mesh::Mesh(Mesh&& m) {
    swap(m);
}

Mesh& Mesh::operator=(Mesh&& m) {
    swap(m);
    return *this;
}

void Mesh::swap(Mesh& m) {
    vs.swap(m.vs);
    is.swap(m.is);
    ts.swap(m.ts);
    std::swap(vao, m.vao);
    std::swap(vbo, m.vbo);
    std::swap(ebo, m.ebo);
}

void Mesh::draw(GpProg& prog) {
    glCheckError();
    int diff_id = 0;
    int spec_id = 0;

    for (size_t i = 0; i < ts.size(); ++i) {
        glCheckError();
        auto& t = ts[i];
        std::string tex_name;

        switch (t->type) {
            case DIFFUSE:
                tex_name = "texture_diffuse[" + std::to_string(diff_id++) + "]";
                break;
            case SPECULAR:
                tex_name = "texture_specular[" + std::to_string(spec_id++) + "]";
                break;
            default:
                DGL_EXC("unknown texture type");
        }

        glUniform1i(prog.uniform_location(tex_name), i);
        glCheckError();

        t->t.bind_as(i);
        glCheckError();
    }

    glActiveTexture(GL_TEXTURE0);

    vao.bind();
    glDrawElements(GL_TRIANGLES, is.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();

    glCheckError();
}

void Mesh::bindVao() {
    vao.bind();
}

std::vector<uint> const& Mesh::indices() const noexcept {
    return is;
}


Model::Model(std::filesystem::path const& p) {
    Assimp::Importer importer;
    aiScene const* scene = importer.ReadFile(p, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DGL_EXC(importer.GetErrorString());
    }

    dir = p.parent_path();

    process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, aiScene const* scene) {
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        ms.push_back(process_mesh(mesh, scene));
    }

    for (uint i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

Mesh Model::process_mesh(aiMesh* mesh, aiScene const* scene) {
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<std::shared_ptr<ModelTexture>> textures;

    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
            glm::vec3 vector; 

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;

            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.texCoords = vec;
            } else {
                vertex.texCoords = glm::vec2(0.0f, 0.0f);
            }

            //vector.x = mesh->mTangents[i].x;
            //vector.y = mesh->mTangents[i].y;
            //vector.z = mesh->mTangents[i].z;
            //vertex.Tangent = vector;

            //vector.x = mesh->mBitangents[i].x;
            //vector.y = mesh->mBitangents[i].y;
            //vector.z = mesh->mBitangents[i].z;
            //vertex.Bitangent = vector;

            vertices.push_back(vertex);
    }

    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (uint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<std::shared_ptr<ModelTexture>> diffuseMaps = 
            load_material_textures(material, aiTextureType_DIFFUSE, DIFFUSE);
        for (auto& x : diffuseMaps)
            textures.push_back(std::move(x));
        std::vector<std::shared_ptr<ModelTexture>> specularMaps =
            load_material_textures(material, aiTextureType_SPECULAR, SPECULAR);
        for (auto& x : specularMaps)
            textures.push_back(std::move(x));
    }

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<std::shared_ptr<ModelTexture>> Model::load_material_textures(aiMaterial* mat
    , aiTextureType type, TextureType ttype) {
    std::vector<std::shared_ptr<ModelTexture>> textures;

    for (uint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::filesystem::path p = dir / str.C_Str();
        auto it = tx_cache.find(p);
        if (it == tx_cache.end()) {
            errlog("Loading texture", dir / str.C_Str());
            auto pr = tx_cache.insert({p, std::make_shared<ModelTexture>(
                        Texture::load_texture2d(p), ttype)});
            textures.push_back(pr.first->second);
        } else {
            textures.push_back(it->second);
        }
    }

    return textures;
}

void Model::draw(GpProg& prog) {
    for (auto& m : ms) {
        m.draw(prog);
    }
}

std::vector<Mesh>& Model::meshes() noexcept {
    return ms;
}

std::vector<Mesh> const& Model::meshes() const noexcept {
    return ms;
}
} // namespace dgl
