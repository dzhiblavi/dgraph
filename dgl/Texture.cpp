#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb/stb_image.h"

namespace dgl {
Texture::Texture(std::filesystem::path const& path, bool sRGB) {
    stbi_set_flip_vertically_on_load(false);
    int width, height, nr_channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);

    if (!data)
        throw std::runtime_error("failed to load image: " + path.string());

    GLenum format;
    GLenum internal_format;

    switch (nr_channels) {
        case 1:
            format = GL_RED;
            internal_format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            internal_format = sRGB ? GL_SRGB : GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            internal_format = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
            break;
        default:
            stbi_image_free(data);
            throw std::runtime_error("unknown number of channels in texture");
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height
            , 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // setting up default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // setting up default filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

Texture::native_handle_t Texture::native_handle() const noexcept {
    return id;
}

Texture& Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, id);
    return *this;
}

Texture& Texture::bind_as(int n) {
    glActiveTexture(GL_TEXTURE0 + n);
    return bind();
}
}
