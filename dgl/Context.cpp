#include "Context.h"

namespace {
dgl::Context* ctx = nullptr;

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    assert(ctx);
    ctx->get_window(window)->key_callback(key, scancode, action, mode);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    assert(ctx);
    ctx->get_window(window)->cur_callback(xpos, ypos);
}
}

namespace dgl {
void Context::add_window(Window* wd) {
    windows[wd->native_handle()] = wd;
}

Context::Context(int argc, char** argv) 
    : args_(argc, argv) {
    glfw_init_();
    ctx = this;
}

Context::~Context() {
    glfwTerminate();
    ctx = nullptr;
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

Context* get_current_context() noexcept {
    return ctx;
}

Window* Context::get_window(Window::window_ptr p) {
    assert(windows.find(p) != windows.end());
    return windows[p];
}

int Context::call_and_timeout() noexcept {
    if (tm.empty()) {
        return -1;
    }

    timer::clock_t::time_point now = timer::clock_t::now();
    tm.callback(now);

    if (tm.empty()) {
        return -1;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(tm.top() - now).count();
}

timer& Context::get_timer() noexcept {
    return tm;
}

void Context::exec() {
    std::for_each(windows.begin(), windows.end(), [] (auto& w) { w.second->run(); });

    while (!windows.empty()) {
        int to = call_and_timeout();
        {
            std::unique_lock<std::mutex> ul(m);
            poll_cv.wait_for(ul, std::chrono::milliseconds(
                        std::max(0, std::min(200, to))));

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
            if (it->second->should_close()) {
                it->second->stop();
                it = windows.erase(it);
                continue;
            }
            ++it;
        }
    }
}

tmnl::cargs const& Context::args() const noexcept {
    return args_;
}

Window::Window(Window::Attributes const& attr)
    : attr(attr)
    , ctx(::ctx) {
    assert(ctx);
    pw = glfw_create_window(attr.width, attr.height, attr.title.c_str(),
                             attr.monitor, attr.share);
    ctx->add_window(this);
    glfwSetKeyCallback(pw, ::key_callback);
    glfwSetCursorPosCallback(pw, ::mouse_callback);

    reg_key_cb([this] (int key, int scancode, int action, int mode) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    });
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

bool Window::pressed(int key) const noexcept {
    return keys[key];
}

void Window::key_callback(int key, int scancode, int action, int mode) {
    key_sig(key, scancode, action, mode);
}

void Window::cur_callback(double xpos, double ypos) {
    cur_sig(xpos, ypos);
}

Window::key_sig_t::connection Window::reg_key_cb(key_cb_t const& cb) {
    return key_sig.connect(cb);
}

Window::cur_sig_t::connection Window::reg_cur_cb(cur_cb_t const& cb) {
    return cur_sig.connect(cb);
}
} // namespace dgl
