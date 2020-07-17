#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <filesystem>
#include <unordered_map>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "error.h"
#include "buffers.h"
#include "Shader.h"

namespace dgl {
struct Character {
    GLuint texId;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    long Advance;
};

class TextRender {
    FT_Library lib;
    std::map<std::string,
        std::pair<std::map<char, Character>, FT_Face>> fonts;
    VAO vao;
    Buffer vbo;

public:
    TextRender();

    ~TextRender();

    void loadFont(std::filesystem::path const& path);

    void renderText(GpProg& s, std::string const& text,
            std::filesystem::path const& font_path,
            GLfloat x, GLfloat y, GLfloat scale,
            glm::vec3 color);
};
} // namespace dgl

