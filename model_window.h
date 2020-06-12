#ifndef DGL_MODEl_WINDOW
#define DGL_MODEl_WINDOW

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dgl/Context.h"
#include "dgl/Shader.h"
#include "dgl/Texture.h"
#include "dgl/error.h"
#include "dgl/buffers.h"
#include "dgl/FpsCamera.h"
#include "dgl/Model.h"
#include "dgl/error.h"
#include "dgl/GpProgFrame.h"
#include "dgl/FrameBuffer.h"

#include "util/cb_signal.h"
#include "util/timer.h"

class model_window : dgl::Window {
private:
    dgl::FpsCamera camera;
    bool outline = false;
    bool normals = false;
    bool pointLight = false;
    float global_size = 1.f;
    float xpos = 26.f, ypos = 24.f, zpos = 20.5f;
    float chspeed = 0.3;

private:
    void process() override;

public:
    model_window(dgl::Window::Attributes attrs);
};

#endif // DGL_MODEl_WINDOW
