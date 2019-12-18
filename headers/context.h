#ifndef DGRAPH_CONTEXT_H
#define DGRAPH_CONTEXT_H

#include <list>
#include <iostream>
#include <string>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace dgl {
class context;

class window {
    friend class context;

public:
    typedef GLFWwindow  window_t;
    typedef GLFWwindow* window_ptr;

    struct attributes {
        int width = 0;
        int height = 0;
        std::string const& title{};
        GLFWmonitor* monitor = nullptr;
        GLFWwindow* share = nullptr;
    };

private:
    context* ctx_ = nullptr;
    attributes* attr_ = nullptr;
    window_ptr pw_ = nullptr;
    std::function<void()> main_loop_{};
    std::thread t_{};

private:
    void process_();
    void run_();
    void stop_();

public:
    window(context& ctx, attributes* attr);

    void swap_buffers();
    void set_default_viewport();
    void set_as_main_context();
    [[nodiscard]] bool should_close();
    [[nodiscard]] window_ptr native_handle();
    void set_process(std::function<void()> main_loop);
};

class context {
    friend class window;

private:
    std::list<window*> vw_;

private:
    void add_window_(window* wd);

public:
    context();

    void exec();
};
}

#endif //DGRAPH_CONTEXT_H
