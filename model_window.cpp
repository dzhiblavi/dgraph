#include "model_window.h"
#include "dgl/FrameBuffer.h"

model_window::model_window(dgl::Window::Attributes attrs)
    : dgl::Window(attrs)
    , camera(*this) {
    glfwSetInputMode(native_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            set_close();
    });

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
            global_size += 0.03f;
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
            global_size -= 0.03f;
    });

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (key == GLFW_KEY_Y && action == GLFW_PRESS)
            xpos += chspeed;
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
            xpos -= chspeed;

        if (key == GLFW_KEY_U && action == GLFW_PRESS)
            ypos += chspeed;
        else if (key == GLFW_KEY_J && action == GLFW_PRESS)
            ypos -= chspeed;

        if (key == GLFW_KEY_I && action == GLFW_PRESS)
            zpos += chspeed;
        else if (key == GLFW_KEY_K && action == GLFW_PRESS)
            zpos -= chspeed;

        if (key == GLFW_KEY_O && action == GLFW_PRESS)
            chspeed += 0.05;
        else if (key == GLFW_KEY_L && action == GLFW_PRESS)
            chspeed -= 0.05;

        //dgl::errlog(xpos, ypos, zpos);
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
        glm::vec3( -1.f,  -1.f,  -1.f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 boxPositions[] = {
        glm::vec3( 5.0f,  4.0f,  0.0f),
        glm::vec3( 2.3f, 3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 7.0f,  2.0f, -3.0f),
        glm::vec3( 11.0f,  1.0f, -5.0f),
    };

    std::array<glm::mat4, 5> boxModels;
    for (size_t i = 0; i < boxModels.size(); ++i) {
        boxModels[i] = glm::translate(glm::mat4(1.f), boxPositions[i]);
    }

    try {
        std::filesystem::path path = std::filesystem::path(ctx->args().getStr("path"))
            .parent_path() / "../";

        dgl::Shader light_vert(path / std::string("assets/shaders/simple/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader light_frag(path / std::string("assets/shaders/simple/white-frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader model_vert(path / std::string("assets/shaders/lights/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader model_frag(path / std::string("assets/shaders/lights/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader dir_shadow_vert(path / std::string("assets/shaders/lights/shadow/directed/vert.glsl"),
                GL_VERTEX_SHADER);
        dgl::Shader dir_shadow_frag(path / std::string("assets/shaders/lights/shadow/directed/frag.glsl"),
                GL_FRAGMENT_SHADER);
        dgl::Shader point_shadow_frag(path / std::string("assets/shaders/lights/shadow/point/frag.glsl"),
                GL_FRAGMENT_SHADER);
        dgl::Shader point_shadow_vert(path / std::string("assets/shaders/lights/shadow/point/vert.glsl"),
                GL_VERTEX_SHADER);
        dgl::Shader text_frag(path / std::string("assets/shaders/text/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader text_vert(path / std::string("assets/shaders/text/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader sb_frag(path / std::string("assets/shaders/skybox/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader sb_vert(path / std::string("assets/shaders/skybox/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader depth_frag(path / std::string("assets/shaders/depth/2d/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader depth_vert(path / std::string("assets/shaders/depth/2d/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader depth_cube_frag(path / std::string("assets/shaders/depth/cube/frag.glsl"), GL_FRAGMENT_SHADER);
        dgl::Shader depth_cube_vert(path / std::string("assets/shaders/depth/cube/vert.glsl"), GL_VERTEX_SHADER);
        dgl::Shader depth_cube_geo(path / std::string("assets/shaders/depth/cube/geo.glsl"), GL_GEOMETRY_SHADER);

        dgl::GpProg light_prog(light_vert, light_frag);
        dgl::GpProg model_prog(model_vert, model_frag);
        dgl::GpProg dir_shadow_prog(dir_shadow_vert, dir_shadow_frag);
        dgl::GpProg point_shadow_prog(point_shadow_vert, point_shadow_frag);
        dgl::GpProg text_prog(text_frag, text_vert);
        dgl::GpProg sb_prog(sb_frag, sb_vert);
        dgl::GpProg depth_prog(depth_frag, depth_vert);
        dgl::GpProg depth_cube_prog(depth_cube_frag, depth_cube_vert, depth_cube_geo);

        dgl::Model lamp(path / "assets/objects/white-box/white-box.obj");
        dgl::Model cyborg(path / "assets/objects/cyborg/cyborg.obj");
        dgl::Model box(path / "assets/objects/box/box.obj");
        dgl::Model ground(path / "assets/objects/brick-ground/brick-ground.obj");

        text_prog.use();
        text_prog["projection"] = glm::ortho(0.0f, 1600.0f, 0.0f, 1200.0f);
        text_prog["text"] = 0;

        dgl::glCheckError();
        glm::mat4 projection(1.f);
        projection = glm::perspective(45.0f, 800.f / 600.f, 0.1f, 100.0f);
        int nd = 0, np = 1, ns = 0;

        light_prog.use();
        light_prog["projection"] = projection;

        sb_prog.use();
        sb_prog["projection"] = projection;

        dir_shadow_prog.use();
        dir_shadow_prog["projection"] = projection;
        dir_shadow_prog["shadowMap"] = 15;

        point_shadow_prog.use();
        point_shadow_prog["projection"] = projection;
        point_shadow_prog["shadowMap"] = 14;
        point_shadow_prog["far_plane"] = 25.0f;

        depth_cube_prog.use();
        depth_cube_prog["farPlane"] = 25.0f;

        model_prog.use();
        model_prog["projection"] = projection;
        model_prog["nDirLights"] = nd;
        model_prog["nPointLights"] = np;
        model_prog["nSpotLights"] = ns;

        auto setup_light = [] (auto light) {
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

        dir_shadow_prog.use();
        auto dlight = dir_shadow_prog["dirLight"];
        setup_light(dlight);
        dlight["direction"] = dirLightsDirections[0];

        point_shadow_prog.use();
        auto plight = point_shadow_prog["pointLight"];
        plight["constant"] = 1.f;
        plight["linear"] = 0.09f;
        plight["quadratic"] = 0.032f;
        setup_light(plight);

        const uint shadowWidth = 1600;
        const uint shadowHeight = 1600;
        dgl::FrameBuffer shadowFB;
        dgl::Texture shadowMap = dgl::Texture::genDepthTex2d(shadowWidth, shadowHeight);
        {
            shadowFB.bind(GL_FRAMEBUFFER);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap.native_handle(), 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            dgl::FrameBuffer::reset(GL_FRAMEBUFFER);
            glActiveTexture(GL_TEXTURE0 + 15);
            shadowMap.bind();
            glActiveTexture(GL_TEXTURE0);
        }

        dgl::FrameBuffer cubeDepthFB;
        dgl::Texture cubeMap = dgl::Texture::genDepthTexCube(shadowWidth, shadowHeight);
        {
            cubeDepthFB.bind(GL_FRAMEBUFFER);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap.native_handle(), 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            dgl::FrameBuffer::reset(GL_FRAMEBUFFER);
            glActiveTexture(GL_TEXTURE0 + 14);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.native_handle());
            glActiveTexture(GL_TEXTURE0);
        }

        float near_plane = 0.1f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 
                (float)shadowWidth / (float)shadowHeight, near_plane, far_plane);

        auto render = [&] (bool drawl, glm::mat4 const& model
                , glm::mat4 const& lmodel, dgl::GpProg& mp, dgl::GpProg& lp) {
            glEnable(GL_DEPTH_TEST);
            {
                if (drawl) {
                    lp.use();
                    lp["model"] = lmodel;
                    lamp.draw(lp);
                }

                mp.use();
                mp["model"] = model;
                ground.draw(mp);
            }

            {
                mp.use();
                mp["model"] = model;
                cyborg.draw(mp);

                for (auto const& m : boxModels) {
                    mp["model"] = m;
                    box.draw(mp);
                }
            }
        };

        //glEnable(GL_MULTISAMPLE);
        //glEnable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        dgl::glCheckError();

        std::chrono::time_point<std::chrono::steady_clock> st = std::chrono::steady_clock::now();
        size_t frames = 0;

        dgl::TextRender textRenderer;

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
                std::vector<glm::mat4> shadowTransforms;
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos 
                            + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos 
                            + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos
                            + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos 
                            + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos 
                            + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
                shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos 
                            + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

                depth_cube_prog.use();
                for (int i = 0; i < 6; ++i) {
                    depth_cube_prog["viewMatrices"][i] = shadowTransforms[i];
                }

                float near_plane = 5.0f, far_plane = 200.f;
                glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                glm::vec3 dirLightPos = glm::vec3(xpos, ypos, zpos);
                glm::mat4 lightView = glm::lookAt(dirLightPos,
                                                dirLightPos + dirLightsDirections[0],
                                                glm::vec3(0.0f, 1.0f, 0.0f));

                glm::mat4 lightSpaceMatrix = lightProjection * lightView;

                dir_shadow_prog.use();
                dir_shadow_prog["lightSpaceMatrix"] = lightSpaceMatrix;

                depth_prog.use();
                depth_prog["lightSpaceMatrix"] = lightSpaceMatrix;
            }

            {
                dir_shadow_prog.use();
                dir_shadow_prog["viewPos"] = cp;
                dir_shadow_prog["view"] = camLookAt;

                model_prog.use();
                model_prog["viewPos"] = cp;
                model_prog["pointLights"][0]["position"] = lightPos;
                model_prog["spotLights"][0]["position"] = cp;
                model_prog["spotLights"][0]["direction"] = camera.direction();
                model_prog["view"] = camLookAt;

                point_shadow_prog.use();
                point_shadow_prog["viewPos"] = cp;
                point_shadow_prog["pointLight"]["position"] = lightPos;
                point_shadow_prog["view"] = camLookAt;

                depth_cube_prog.use();
                depth_cube_prog["lightPos"] = lightPos;

                light_prog.use();
                light_prog["view"] = camLookAt;
                light_prog["model"] = light_model;
            }

            glClearColor(0.f, 0.f, 0.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // directional light shadow pass
            //glCullFace(GL_FRONT);
            //{
                //glViewport(0, 0, shadowWidth, shadowHeight);
                //shadowFB.bind(GL_FRAMEBUFFER);
                //glClear(GL_DEPTH_BUFFER_BIT);
                //render(false, model, light_model, depth_prog, depth_prog);
            //}
            //glCullFace(GL_BACK);

            // point light shadow pass
            {
                glViewport(0, 0, shadowWidth, shadowHeight);
                cubeDepthFB.bind(GL_FRAMEBUFFER);
                glClear(GL_DEPTH_BUFFER_BIT);
                render(false, model, light_model, depth_cube_prog, depth_cube_prog);
            }

            // render pass
            set_default_viewport();
            dgl::FrameBuffer::reset(GL_FRAMEBUFFER);
            //render(true, model, light_model, model_prog, light_prog);
            render(true, model, light_model, point_shadow_prog, light_prog);

            // fps
            glDisable(GL_DEPTH_TEST);
            std::chrono::duration<double> dur = std::chrono::steady_clock::now() - st;
            double FPS = (1. * frames) / dur.count();
            textRenderer.renderText(text_prog, "Avg. fps = " + std::to_string(FPS),
                    "/Library/fonts/Arial Unicode.ttf", 25.0f, 250.0f, 1.0f, glm::vec3(1.f));
            glEnable(GL_DEPTH_TEST);
            // -- fps

            swap_buffers();
            dgl::glCheckError();
        }


        dgl::glCheckError();
    } catch (std::runtime_error const& re) {
        std::cerr << "Error occured in loop: " << re.what() << std::endl;
    } catch (...) {
        std::cerr << "Error occured in loop" << std::endl;
    }
}

