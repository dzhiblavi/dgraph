#ifndef DGL_FPS_CAMERA_H
#define DGL_FPS_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Context.h"

namespace dgl {
class FpsCamera {
private:
    Window* window;
    bool first_mouse = true;
    GLfloat lastX = 400, lastY = 300;
    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

public:
    FpsCamera(Window& w);

    FpsCamera& apply_move();

    glm::mat4 lookAt() const noexcept;

    glm::vec3& position() noexcept;

    glm::vec3& direction() noexcept;
};
} // namespace dgl

#endif // DGL_FPS_CAMERA_H
