#include "../headers/context.h"

namespace {
#ifdef DEBUG
std::mutex cerr_m;

void errlog_impl_() {
    std::cerr << std::endl;
}

template <typename T, typename... Args>
void errlog_impl_(T&& t, Args&&... args) {
    std::cerr << t << " ";
    errlog_impl_(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void errlog(T&& t, Args&&... args) {
    std::unique_lock<std::mutex> lg(cerr_m);
    errlog_impl_(std::forward<T>(t), std::forward<Args>(args)...);
}
#else
void errlog(...) {}
#endif

GLFWwindow* glfw_create_window(int w, int h, char const* name, GLFWmonitor* monitor, GLFWwindow* share) {
    GLFWwindow* wd = glfwCreateWindow(w, h, name, monitor, share);
    if (!wd)
        throw std::runtime_error("failed to create window");
    return wd;
}

void glfw_init_() {
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("failed to init glfw");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

bool glew_was_initialized = false;

void glew_init_() {
    if (glew_was_initialized) return;
    glew_was_initialized = true;
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
    for (auto w : vw_)
        w->run_();

//    std::chrono::time_point base = std::chrono::steady_clock::now();
    while (!vw_.empty()) {
//        std::chrono::time_point now = std::chrono::steady_clock::now();
//        std::chrono::duration<double> tmp = std::chrono::steady_clock::now() - base;
//        base = now;
//        errlog("glfwPoll(), fps = ", 1.f / tmp.count());

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        glfwPollEvents();
        for (auto it = vw_.begin(); it != vw_.end(); ) {
            if ((*it)->should_close()) {
                (*it)->stop_();
                it = vw_.erase(it);
                continue;
            }
            ++it;
        }
    }

    glfwTerminate();
}

window::window(context &ctx, window::attributes *attr)
    : ctx_(&ctx)
    , attr_(attr)
{
    ctx.add_window_(this);
}

void window::run_() {
    errlog("run()", this);

    t_ = std::thread([this] {
        pw_ = glfw_create_window(attr_->width, attr_->height, attr_->title.c_str(),
                                 attr_->monitor, attr_->share);
        set_as_main_context();
        set_default_viewport();
        glew_init_();

        errlog("main_loop()", this);
        process_();
    });
}

void window::stop_() {
    errlog("stop()", this);

    t_.join();
    glfwDestroyWindow(pw_);
}

void window::swap_buffers() {
    glfwSwapBuffers(pw_);
}

void window::process_() {
    assert(main_loop_);
    main_loop_();
}

void window::set_default_viewport() {
    int width, height;
    glfwGetFramebufferSize(pw_, &width, &height);
    glViewport(0, 0, width, height);
}

void window::set_as_main_context() {
    glfwMakeContextCurrent(pw_);
}

void window::set_process(std::function<void()> main_loop) {
    main_loop_.swap(main_loop);
}

bool window::should_close() {
    if (!pw_) return false;
    return glfwWindowShouldClose(pw_);
}

GLFWwindow* window::native_handle() {
    return pw_;
}
}