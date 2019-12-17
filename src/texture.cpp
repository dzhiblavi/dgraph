#include "../headers/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stbi_image.hpp"

namespace {

}

namespace dgl {
texture2d::texture2d(std::string const& path, bool sRGB) {
    stbi_set_flip_vertically_on_load(false);
    int width, height, nr_channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (!data)
        throw std::runtime_error("failed to load image");

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

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // setting up default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}

texture2d::~texture2d() {
    glDeleteTextures(1, &id_);
}

GLuint texture2d::native_handle() {
    return id_;
}

void texture2d::bind() {
    glBindTexture(GL_TEXTURE_2D, id_);
}
}