#include "app_window.h"
#include "dgl/error.h"

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
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 dirLightsDirections[] = {
        glm::vec3( 0.0f,  -1.0f,  0.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    try {
        glEnable(GL_DEPTH_TEST);

        dgl::VBO vbo;
        dgl::VAO vao;

        vao.bind();
        vbo.bind()
            .buffer_data(vertices, sizeof(vertices), GL_STATIC_DRAW);

        vao.setup_attribute({0, 3, GL_FLOAT, GL_FALSE
                    , 8 * sizeof(float), (void *) nullptr})
            .setup_attribute({1, 3, GL_FLOAT, GL_FALSE
                    , 8 * sizeof(float), (void *) (3 * sizeof(float))})
            .setup_attribute({2, 2, GL_FLOAT, GL_FALSE
                    , 8 * sizeof(float), (void *) (6 * sizeof(float))})
            .enable_attribute(0)
            .enable_attribute(1)
            .enable_attribute(2);

        dgl::glCheckError();

        std::filesystem::path path = std::filesystem::path(ctx->args().getStr("path"))
            .parent_path() / "../";

        dgl::Shader light_vert(path / std::string("assets/shaders/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader light_frag(path / std::string("assets/shaders/light-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg light_prog(light_vert, light_frag);

        dgl::Shader phong_vert(path / std::string("assets/shaders/phong/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader phong_frag(path / std::string("assets/shaders/phong/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg phong_prog(phong_vert, phong_frag);

        dgl::glCheckError();

        //
        dgl::Texture box_diffuse(path / "assets/textures/box-diff.png");
        dgl::Texture box_specular(path / "assets/textures/box-spec.png");

        box_diffuse.bind_as(0);
        box_specular.bind_as(1);
        //

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
        phong_prog.use();

        GLuint modelid = phong_prog.uniform_location("model");
        GLuint viewid = phong_prog.uniform_location("view");
        GLuint projid = phong_prog.uniform_location("projection");
        GLuint viewPos = phong_prog.uniform_location("viewPos");
        glUniformMatrix4fv(projid, 1, GL_FALSE, glm::value_ptr(projection));

        GLuint ndir = phong_prog.uniform_location("nDirLights");
        GLuint npoint = phong_prog.uniform_location("nPointLights");
        GLuint nspot = phong_prog.uniform_location("nSpotLights");

        int nd = 1;
        int np = 1;
        int ns = 1;
        glUniform1i(ndir, nd);
        glUniform1i(npoint, np);
        glUniform1i(nspot, ns);

        GLuint material_diffuse = phong_prog.uniform_location("material.diffuse");
        GLuint material_specular = phong_prog.uniform_location("material.specular");
        GLuint material_shininess = phong_prog.uniform_location("material.shininess");
        glUniform1f(material_shininess, 64.f);
        glUniform1i(material_diffuse, 0);
        glUniform1i(material_specular, 1);

        for (int i = 0; i < nd; ++i) {
            GLuint light_direction = phong_prog.uniform_location("dirLights[" + std::to_string(i) + "].direction");
            GLuint light_ambient = phong_prog.uniform_location("dirLights[" + std::to_string(i) + "].ambient");
            GLuint light_diffuse = phong_prog.uniform_location("dirLights[" + std::to_string(i) + "].diffuse");
            GLuint light_specular = phong_prog.uniform_location("dirLights[" + std::to_string(i) + "].specular");

            glUniform3fv(light_direction, 1, glm::value_ptr(dirLightsDirections[i]));
            glUniform3f(light_ambient, 0.05f, 0.05f, 0.05f);
            glUniform3f(light_diffuse, 0.8f, 0.8f, 0.8f);
            glUniform3f(light_specular, 1.0f, 1.0f, 1.0f);
        }

        for (int i = 0; i < np; ++i) {
            GLuint light_position = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].position");
            GLuint light_constant = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].constant");
            GLuint light_linear = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].linear");
            GLuint light_quadratic = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].quadratic");
            GLuint light_ambient = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].ambient");
            GLuint light_diffuse = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].diffuse");
            GLuint light_specular = phong_prog.uniform_location("pointLights[" + std::to_string(i) + "].specular");

            glUniform3fv(light_position, 1, glm::value_ptr(pointLightPositions[i]));
            glUniform1f(light_constant, 1.0f);
            glUniform1f(light_linear, 0.09f);
            glUniform1f(light_quadratic, 0.032f);
            glUniform3f(light_ambient, 0.05f, 0.05f, 0.05f);
            glUniform3f(light_diffuse, 0.8f, 0.8f, 0.8f);
            glUniform3f(light_specular, 1.0f, 1.0f, 1.0f);
        }

        int i = 0;
        GLuint light_position = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].position");
        GLuint light_direction = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].direction");
        GLuint light_cutoff = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].cutOff");
        GLuint light_outerCutoff = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].outerCutOff");
        GLuint light_constant = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].constant");
        GLuint light_linear = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].linear");
        GLuint light_quadratic = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].quadratic");
        GLuint light_ambient = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].ambient");
        GLuint light_diffuse = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].diffuse");
        GLuint light_specular = phong_prog.uniform_location("spotLights[" + std::to_string(i) + "].specular");

        glUniform1f(light_cutoff, glm::cos(glm::radians(12.5f)));
        glUniform1f(light_outerCutoff, glm::cos(glm::radians(15.0f)));
        glUniform1f(light_constant, 1.0f);
        glUniform1f(light_linear, 0.09f);
        glUniform1f(light_quadratic, 0.032f);
        glUniform3f(light_ambient, 0.05f, 0.05f, 0.05f);
        glUniform3f(light_diffuse, 0.8f, 0.8f, 0.8f);
        glUniform3f(light_specular, 1.0f, 1.0f, 1.0f);

        dgl::glCheckError();

        GLuint light0_position = phong_prog.uniform_location("pointLights[0].position");

        while (!should_close()) {
            poll_events();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            vao.bind();

            //
            light_prog.use();
            glUniformMatrix4fv(lviewid, 1, GL_FALSE, glm::value_ptr(camera.apply_move().lookAt()));

            double time = glfwGetTime();
            glm::vec3 lightPos = glm::vec3(sin(time), 2.0f, cos(time));

            glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.f), lightPos), glm::vec3(0.2f));
            glUniformMatrix4fv(lmodelid, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);

            //
            phong_prog.use();

            glUniformMatrix4fv(viewid, 1, GL_FALSE, glm::value_ptr(camera.lookAt()));

            auto& cp = camera.position();
            glUniform3f(viewPos, cp.x, cp.y, cp.z);

            glUniform3fv(light0_position, 1, glm::value_ptr(lightPos));

            glUniform3fv(light_position, 1, glm::value_ptr(cp));
            glUniform3fv(light_direction, 1, glm::value_ptr(camera.direction()));

            for (int i = 0; i < 10; ++i) {
                glm::mat4 model(1.f);
                model = glm::translate(model, cubePositions[i]);
                GLfloat angle = 20.0f * i;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
                glUniformMatrix4fv(modelid, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

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

