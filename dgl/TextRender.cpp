#include "TextRender.h"

namespace dgl {
TextRender::TextRender() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (FT_Init_FreeType(&lib))
        DGL_EXC("Failed to initialize FreeType");

    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    Buffer::bufferData(GL_ARRAY_BUFFER, nullptr, sizeof(GL_FLOAT) * 4 * 6, GL_DYNAMIC_DRAW);

    VAO::setupAttribute(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), nullptr);
    VAO::enableAttribute(0);
    VAO::setupAttribute(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
    VAO::enableAttribute(1);

    Buffer::unbind(GL_ARRAY_BUFFER);
    VAO::unbind();
}

TextRender::~TextRender() {
    FT_Done_FreeType(lib);

    for (auto const& ps : fonts) {
        for (auto const& m : ps.second.first) {
            glDeleteTextures(1, &m.second.texId);
        }
    }
}

void TextRender::loadFont(std::filesystem::path const& path) {
    FT_Face face;
    if (FT_New_Face(lib, path.string().c_str(), 0, &face)) {
        DGL_EXC("Failed to load font: " + path.string());
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    std::map<char, Character> characters;
    for (GLubyte c = 0; c < 128; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            DGL_EXC("Failed to load glyph #" + std::to_string(c));
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character;
        character.texId = texture;
        character.Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.Advance = face->glyph->advance.x;
        characters[c] = character;
    }

    fonts[path.string()] = {characters, face};
    FT_Done_Face(face);
}

void TextRender::renderText(GpProg& s, std::string const& text,
            std::filesystem::path const& font_path,
            GLfloat x, GLfloat y, GLfloat scale,
            glm::vec3 color) {
    if (fonts.find(font_path.string()) == fonts.end()) {
        loadFont(font_path);
    }

    s.use();
    s["textColor"] = color;

    glActiveTexture(GL_TEXTURE0);
    vao.bind();

    std::map<char, Character>& characters = fonts[font_path.string()].first;

    for (char c : text) {
        Character ch = characters[c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        GLfloat vertices[6][4] = {
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos, ypos, 0.0, 1.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos + w, ypos + h, 1.0, 0.0 }
        };
        
        vbo.bind(GL_ARRAY_BUFFER);
        Buffer::bufferSubData(GL_ARRAY_BUFFER, vertices, sizeof(vertices), 0);
        Buffer::unbind(GL_ARRAY_BUFFER);

        glBindTexture(GL_TEXTURE_2D, ch.texId);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }

    VAO::unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespace dgl

