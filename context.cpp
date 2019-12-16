#include "context.h"

namespace {
GLFWwindow* glfw_create_window(int w, int h, char const* name, GLFWmonitor* monitor, GLFWwindow* share) {
    GLFWwindow* wd = glfwCreateWindow(w, h, name, monitor, share);
    if (!wd)
        throw std::runtime_error("failed to create window");
    return wd;
}

void glfw_init_() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void glew_init_() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("glew_init() failed");
}
}

namespace dgl {
void context::add_window_(dgl::window *wd) {
    vw_.push_back(wd);
}

context::context() {
    glfw_init_();
}

context::~context() {
    glfwTerminate();
}

void context::exec() {
    assert(!vw_.empty());
    glew_init_();

    while (!vw_.empty()) {
        for (auto it = vw_.begin(); it != vw_.end();) {
            glfwPollEvents();
            if ((*it)->should_close()) {
                (*it)->close_();
                it = vw_.erase(it);
                continue;
            }
            (*it)->process_();
            ++it;
        }
    }
}

window::render_thread_::render_thread_(std::function<void()> &&f)
    : t(std::move(f))
{}

window::render_thread_::~render_thread_() {
    if (!quit)
        stop();
}

void window::render_thread_::stop() {
    {
        std::unique_lock<std::mutex> lg(m);
        quit = true;
    }
    cv.notify_one();
    t.join();
}

window::window(context &ctx, window::attributes *attr)
    : pw_(glfw_create_window(attr->width, attr->height, attr->title.c_str(), attr->monitor, attr->share))
    , wt_(
    [this] {
        set_as_main_context();
        for (;;) {
            std::unique_lock<std::mutex> lg(wt_.m);
            wt_.cv.wait(lg, [this] {
                return wt_.quit || (wt_.work && fproc_);
            });
            if (wt_.quit) break;
            lg.unlock();

            fproc_();
            swap_buffers();
            wt_.work = false;
        }
    })
{
    ctx.add_window_(this);
    set_as_main_context();
    set_default_viewport();
}

void window::swap_buffers() {
    glfwSwapBuffers(pw_);
}

void window::set_default_viewport() {
    int width, height;
    glfwGetFramebufferSize(pw_, &width, &height);
    glViewport(0, 0, width, height);
}

void window::set_as_main_context() {
    glfwMakeContextCurrent(pw_);
}

void window::process_() {
    {
        std::unique_lock<std::mutex> lg(wt_.m);
        wt_.work = true;
    }
    wt_.cv.notify_one();
}

void window::close_() {
    wt_.stop();
    glfwDestroyWindow(pw_);
}

void window::set_process(std::function<void()> fproc) {
    fproc_.swap(fproc);
}

bool window::should_close() {
    return glfwWindowShouldClose(pw_);
}
}