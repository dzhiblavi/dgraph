#include "Context.h"

namespace {
GLFWwindow* glfw_create_window(int w, int h, char const* name
        , GLFWmonitor* monitor, GLFWwindow* share) {
    return glfwCreateWindow(w, h, name, monitor, share);
}

void glfw_init_() {
    glfwSetErrorCallback(dgl::glfwErrorCallback);
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void glew_init_() {
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
    dgl::glCheckError();

//#ifdef DEBUG
//    glEnable(GL_DEBUG_OUTPUT);
//    glDebugMessageCallback(dgl::glErrorCallback, nullptr);
//#endif
}
}

namespace dgl {
void Context::add_window(Window* wd) {
    windows.push_back(wd);
}

Context::Context() {
    glfw_init_();
}

Context::~Context() {
    glfwTerminate();
}

void Context::poll_events() {
    poll_cv.notify_one();
}

void Context::submit(std::function<void()> const& f) {
    {
        std::lock_guard<std::mutex> lg(m);
        q.push(f);
    }
    poll_cv.notify_one();
}

void Context::exec() {
    std::for_each(windows.begin(), windows.end(), [] (auto& w) { w->run(); });

    while (!windows.empty()) {
        {
            std::unique_lock<std::mutex> ul(m);
            poll_cv.wait_for(ul, std::chrono::milliseconds(200));

            while (!q.empty()) {
                auto f = q.front(); q.pop();
                try {
                    f();
                } catch (...) {
                    // ignore
                }
            }
        }
        glfwPollEvents();

        for (auto it = windows.begin(); it != windows.end(); ) {
            if ((*it)->should_close()) {
                (*it)->stop();
                it = windows.erase(it);
                continue;
            }
            ++it;
        }
    }
}

Window::Window(Context &ctx, Window::Attributes const& attr
        , std::function<void()> const& loop)
    : attr(attr)
    , ctx(&ctx)
    , main_loop(loop) {
    pw = glfw_create_window(attr.width, attr.height, attr.title.c_str(),
                             attr.monitor, attr.share);
    ctx.add_window(this);
}

Window::Window(Window&& w) {
    swap(w);
}

Window& Window::operator=(Window&& w) {
    if (this != &w) {
        swap(w);
    }
    return *this;
}

void Window::swap(Window& w) {
    std::swap(attr, w.attr);
    std::swap(ctx, w.ctx);
    std::swap(pw, w.pw);
    std::swap(main_loop, w.main_loop);
    std::swap(th, w.th);
}

void Window::run() {
    errlog("run()", this);

    th = std::thread([this] {
        try {
            set_as_main_context();
            glew_init_();
            set_default_viewport();

            errlog("main_loop()", this);
            process();

            glfwMakeContextCurrent(nullptr);
        } catch (std::runtime_error const& re) {
            std::cerr << "Error in window thread:" << re.what() << std::endl;
        } catch (...) {
            std::cerr << "Error in window thread." << std::endl;
        }

        set_close(1);
    });
}

void Window::poll_events() {
    assert(ctx);
    ctx->poll_events();
}

void Window::stop() {
    errlog("stop()", this);

    th.join();
    glfwDestroyWindow(pw);
}

void Window::swap_buffers() {
    assert(pw);
    glfwSwapBuffers(pw);
}

void Window::process() {
    assert(main_loop);
    main_loop();
}

void Window::set_default_viewport() {
    int width, height;
    glfwGetFramebufferSize(pw, &width, &height);
    glViewport(0, 0, width, height);
}

void Window::set_as_main_context() {
    glfwMakeContextCurrent(pw);
}

void Window::set_close(int flag) {
    glfwSetWindowShouldClose(pw, flag);
}

bool Window::should_close() {
    if (!pw) return false;
    return glfwWindowShouldClose(pw);
}

GLFWwindow* Window::native_handle() {
    return pw;
}
}
