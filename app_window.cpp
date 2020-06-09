#include "app_window.h"

window::window(dgl::Window::Attributes attrs)
    : dgl::Window(attrs)
    , camera(*this) {
    glfwSetInputMode(native_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            set_close();
    });
}

void window::process() {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::vec3 lightPos(0.0f, 2.0f, 0.0f);
    glm::vec3 cubePos(-1.0f, 0.0f, 1.0f);

    try {
        glEnable(GL_DEPTH_TEST);

        dgl::VBO vbo;
        dgl::VAO vao;

        vao.bind();
        vbo.bind()
            .buffer_data(vertices, sizeof(vertices), GL_STATIC_DRAW);

        vao.setup_attribute({0, 3, GL_FLOAT, GL_FALSE
                    , 6 * sizeof(float), (void *) nullptr})
            .setup_attribute({1, 3, GL_FLOAT, GL_FALSE
                    , 6 * sizeof(float), (void *) (3 * sizeof(float))})
            .enable_attribute(0)
            .enable_attribute(1);

        dgl::glCheckError();

        std::filesystem::path path = std::filesystem::path(ctx->args().getStr("path"))
            .parent_path() / "../";

        dgl::Shader light_vert(path / std::string("assets/shaders/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader light_frag(path / std::string("assets/shaders/light-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg light_prog(light_vert, light_frag);

        dgl::Shader color_vert(path / std::string("assets/shaders/vert-normal.glsl"), GL_VERTEX_SHADER);
        dgl::Shader color_frag(path / std::string("assets/shaders/color-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg color_prog(color_vert, color_frag);

        dgl::glCheckError();

        //
        light_prog.use();

        glm::mat4 projection(1.f);
        projection = glm::perspective(45.0f, 800.f / 600.f, 0.1f, 100.0f);

        GLuint lmodelid = light_prog.uniform_location("model");
        GLuint lviewid = light_prog.uniform_location("view");
        GLuint lprojid = light_prog.uniform_location("projection");

        glUniformMatrix4fv(lprojid, 1, GL_FALSE, glm::value_ptr(projection));
        //

        //
        color_prog.use();

        GLuint cmodelid = color_prog.uniform_location("model");
        GLuint cviewid = color_prog.uniform_location("view");
        GLuint cprojid = color_prog.uniform_location("projection");
        GLuint cobjclrid = color_prog.uniform_location("objectColor");
        GLuint clightclrid = color_prog.uniform_location("lightColor");
        GLuint clightpos = color_prog.uniform_location("lightPos");

        glUniformMatrix4fv(cprojid, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(cobjclrid, 1.0f, 0.5f, 0.31f);
        glUniform3f(clightclrid,  1.0f, 1.0f, 1.0f);
        glUniform3f(clightpos, lightPos.x, lightPos.y, lightPos.z);
        //

        dgl::glCheckError();

        while (!should_close()) {
            poll_events();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            vao.bind();

            //
            color_prog.use();
            glUniformMatrix4fv(cviewid, 1, GL_FALSE, glm::value_ptr(camera.apply_move().lookAt()));

            glm::mat4 model = glm::translate(glm::mat4(1.f), cubePos);
            glUniformMatrix4fv(cmodelid, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //

            //
            light_prog.use();
            glUniformMatrix4fv(lviewid, 1, GL_FALSE, glm::value_ptr(camera.apply_move().lookAt()));

            model = glm::scale(glm::translate(glm::mat4(1.f), lightPos), glm::vec3(0.2f));
            glUniformMatrix4fv(lmodelid, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            //

            glBindVertexArray(0);
            swap_buffers();
        }

        dgl::glCheckError();
    } catch (std::runtime_error const& re) {
        std::cerr << "Error occured in loop: " << re.what() << std::endl;
    } catch (...) {
        std::cerr << "Error occured in loop" << std::endl;
    }
}

