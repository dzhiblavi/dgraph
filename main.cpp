#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "headers/context.h"
#include "headers/shader.h"
#include "headers/texture.h"

char const* path = "/home/dzhiblavi/Documents/prog/cpp/code/dgraph/";

bool keys[1024], first_mouse = true;
GLfloat lastX = 400, lastY = 300;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

std::function<void()> do_move;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (first_mouse) {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f)
        pitch =  89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

void main_window_loop(dgl::window& wd) {
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

    glfwSetKeyCallback(wd.native_handle(), key_callback);
    glfwSetInputMode(wd.native_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(wd.native_handle(), mouse_callback);
    glEnable(GL_DEPTH_TEST);

//    GLuint EBO;
//    glGenBuffers(1, &EBO);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


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

    glm::mat4 projection(1.f), view;
    projection = glm::perspective(45.0f, 800.f / 600.f, 0.1f, 100.0f);

    GLuint modelid = def_prog.uniform_location("model");
    GLuint viewid = def_prog.uniform_location("view");
    GLuint projid = def_prog.uniform_location("projection");

    glUniformMatrix4fv(projid, 1, GL_FALSE, glm::value_ptr(projection));

    do_move = [&] {
        GLfloat cameraSpeed = 0.05f;
        if (keys[GLFW_KEY_W])
            cameraPos += cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_S])
            cameraPos -= cameraSpeed * cameraFront;
        if (keys[GLFW_KEY_A])
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (keys[GLFW_KEY_D])
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    };

    while (!wd.should_close()) {
        glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        do_move();
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewid, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);
        for(GLuint i = 0; i < 10; i++) {
            glm::mat4 model(1.f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelid, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        wd.swap_buffers();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
}

int main() {
    try {
        dgl::context ctx;
        dgl::window::attributes attr{1600, 1200, "test window 1", nullptr, nullptr};

        dgl::window w1(ctx, &attr);
        w1.set_process([&w1] { main_window_loop(w1); });

        ctx.exec();
    } catch (std::exception const& e) {
        std::cerr << "main() failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "main() failed with unknown exception" << std::endl;
    }
    return 0;
}