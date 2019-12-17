#include <iostream>

#include "headers/context.h"
#include "headers/shader.h"
#include "headers/texture.h"

char const* path = "/home/dzhiblavi/Documents/prog/cpp/code/dgraph/";

void main_window_loop(dgl::window& wd) {
    GLfloat vertices[] = {
        //      pos                 clr             texture
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    GLuint indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    GLuint EBO;
    glGenBuffers(1, &EBO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    dgl::shader def_vert(path + std::string("assets/shaders/vert.glsl"), GL_VERTEX_SHADER);
    dgl::shader def_frag(path + std::string("assets/shaders/frag.glsl"), GL_FRAGMENT_SHADER);
    dgl::gp_prog def_prog({def_vert, def_frag});

    dgl::texture2d brick(path + std::string("assets/textures/container.jpg"));
    dgl::texture2d face(path + std::string("assets/textures/face.png"));

    glActiveTexture(GL_TEXTURE0);
    brick.bind();
    glActiveTexture(GL_TEXTURE1);
    face.bind();

    def_prog.use();

    GLuint tex1loc = def_prog.uniform_location("ourTexture1");
    glUniform1i(tex1loc, 0);
    GLuint tex2loc = def_prog.uniform_location("ourTexture2");
    glUniform1i(tex2loc, 1);

    while (!wd.should_close()) {
        glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        wd.swap_buffers();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

int main() {
    try {
        dgl::context ctx;
        dgl::window::attributes attr{800, 600, "test window 1", nullptr, nullptr};

        dgl::window w1(ctx, &attr);
        w1.set_process([&w1] { main_window_loop(w1); });
    } catch (std::exception const& e) {
        std::cerr << "main() failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "main() failed with unknown exception" << std::endl;
    }
    return 0;
}