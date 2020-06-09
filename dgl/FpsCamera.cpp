#include "FpsCamera.h"

namespace dgl {
FpsCamera::FpsCamera(Window& w) 
    : window(&w) {
    window->reg_cur_cb([this] (double xpos, double ypos) {
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
    });
}

FpsCamera& FpsCamera::apply_move() {
    GLfloat cameraSpeed = 0.05f;
    if (window->pressed(GLFW_KEY_W))
        cameraPos += cameraSpeed * cameraFront;
    if (window->pressed(GLFW_KEY_S))
        cameraPos -= cameraSpeed * cameraFront;
    if (window->pressed(GLFW_KEY_A))
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (window->pressed(GLFW_KEY_D))
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    return *this;
}

glm::mat4 FpsCamera::lookAt() const noexcept {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
} // namespace dgl
