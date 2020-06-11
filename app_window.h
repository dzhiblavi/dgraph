#ifndef DGL_APP_WINDOW
#define DGL_APP_WINDOW

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dgl/Context.h"
#include "dgl/Shader.h"
#include "dgl/Texture.h"
#include "dgl/error.h"
#include "dgl/buffers.h"
#include "dgl/FpsCamera.h"

class app_window : dgl::Window {
private:
    dgl::FpsCamera camera;

private:
    void process() override;

public:
    app_window(dgl::Window::Attributes attrs);
};

#endif // DGL_APP_WINDOW
