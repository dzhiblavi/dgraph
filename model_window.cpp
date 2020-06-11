#include "model_window.h"

model_window::model_window(dgl::Window::Attributes attrs)
    : dgl::Window(attrs)
    , camera(*this) {
    glfwSetInputMode(native_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            set_close();
    });

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_T && action == GLFW_PRESS)
            outline ^= 1;
    });

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_N && action == GLFW_PRESS)
            normals ^= 1;
    });

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
            global_size += 0.03f;
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
            global_size -= 0.03f;
    });
}

void model_window::process() {
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
        std::filesystem::path path = std::filesystem::path(ctx->args().getStr("path"))
            .parent_path() / "../";

        dgl::Shader light_vert(path / std::string("assets/shaders/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader light_frag(path / std::string("assets/shaders/light-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg light_prog(light_vert, light_frag);

        dgl::Shader model_vert(path / std::string("assets/shaders/model/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader model_frag(path / std::string("assets/shaders/model/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::GpProg model_prog(model_vert, model_frag);

        dgl::Shader norm_vert(path / std::string("assets/shaders/model/normal-vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader norm_frag(path / std::string("assets/shaders/model/normal-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader norm_geo(path / std::string("assets/shaders/model/normal-geo.glsl"), GL_GEOMETRY_SHADER);
        dgl::GpProg norm_prog(norm_vert, norm_frag, norm_geo);

        dgl::glCheckError();

        glm::mat4 projection(1.f);
        projection = glm::perspective(45.0f, 800.f / 600.f, 0.1f, 100.0f);

        dgl::Model lamp(path / "assets/objects/white-box/white-box.obj");
        dgl::Model cyborg(path / "assets/objects/cyborg/cyborg.obj");
        dgl::Model box(path / "assets/objects/box/box.obj");
        dgl::Model ground(path / "assets/objects/brick-ground/brick-ground.obj");

        int nd = 1, np = 1, ns = 1;

        light_prog.use();
        light_prog["projection"] = projection;

        model_prog.use();
        model_prog["projection"] = projection;
        model_prog["nDirLights"] = nd;
        model_prog["nPointLights"] = np;
        model_prog["nSpotLights"] = ns;

        norm_prog.use();
        norm_prog["projection"] = projection;

        auto setup_light = [] (auto& light) {
            light["ambient"] = glm::vec3(0.05f, 0.05f, 0.05f);
            light["diffuse"] = glm::vec3(0.8f, 0.8f, 0.8f);
            light["specular"] = glm::vec3(1.f, 1.f, 1.f);
        };

        model_prog.use();
        for (int i = 0; i < nd; ++i) {
            auto light = model_prog["dirLights"][i];
            light["direction"] = dirLightsDirections[i];
            setup_light(light);
        }

        for (int i = 0; i < np; ++i) {
            auto light = model_prog["pointLights"][i];
            light["position"] = pointLightPositions[i];
            light["constant"] = 1.f;
            light["linear"] = 0.09f;
            light["quadratic"] = 0.032f;
            setup_light(light);
        }

        int i = 0;
        auto spotLight = model_prog["spotLights"][i];
        setup_light(spotLight);
        spotLight["cutOff"] = glm::cos(glm::radians(12.5f));
        spotLight["outerCutOff"] = glm::cos(glm::radians(15.f));
        spotLight["constant"] = 1.f;
        spotLight["linear"] = 0.09f;
        spotLight["quadratic"] = 0.032f;

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        dgl::glCheckError();

        std::chrono::time_point<std::chrono::steady_clock> st = std::chrono::steady_clock::now();
        size_t frames = 0;
        while (!should_close()) {
            ++frames;
            double time = glfwGetTime();
            auto& cp = camera.position();
            glm::vec3 lightPos;
            glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(global_size));
            poll_events();
            lightPos = glm::vec3(sin(time), 2.0f, cos(time));
            glm::mat4 light_model = glm::scale(glm::translate(glm::mat4(1.f), lightPos), glm::vec3(0.2f));
            glm::mat4 camLookAt = camera.apply_move().lookAt();

            {
                model_prog.use();
                model_prog["viewPos"] = cp;
                model_prog["pointLights"][0]["position"] = lightPos;
                model_prog["spotLights"][0]["position"] = cp;
                model_prog["spotLights"][0]["direction"] = camera.direction();
                model_prog["model"] = model;
                model_prog["view"] = camLookAt;

                light_prog.use();
                light_prog["view"] = camLookAt;
                light_prog["model"] = light_model;

                norm_prog.use();
                norm_prog["model"] = model;
                norm_prog["view"] = camLookAt;
            }

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glStencilMask(0x00);
            {
                light_prog.use();
                lamp.draw(light_prog);

                model_prog.use();
                ground.draw(model_prog);
            }

            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            {
                model_prog.use();
                cyborg.draw(model_prog);

                if (normals) {
                    norm_prog.use();
                    cyborg.draw(norm_prog);
                }
            }

            if (outline) {
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);
                glDisable(GL_DEPTH_TEST);
                {
                    light_prog.use();
                    light_prog["model"] = glm::scale(model, glm::vec3(1.01f));
                    cyborg.draw(light_prog);
                }
                glStencilMask(0xFF);
                glEnable(GL_DEPTH_TEST);
            }

            swap_buffers();
            dgl::glCheckError();
        }

        std::chrono::duration<double> dur = std::chrono::steady_clock::now() - st;
        std::cout << "FPS: " << (1. * frames) / dur.count() << std::endl;

        dgl::glCheckError();
    } catch (std::runtime_error const& re) {
        std::cerr << "Error occured in loop: " << re.what() << std::endl;
    } catch (...) {
        std::cerr << "Error occured in loop" << std::endl;
    }
}

