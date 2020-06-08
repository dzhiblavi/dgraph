#ifndef DGRAPH_CONTEXT_H
#define DGRAPH_CONTEXT_H

#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <mutex>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "dgl/error.h"

namespace dgl {
class Context;

class Window {
    friend class Context;

public:
    typedef GLFWwindow  window_t;
    typedef GLFWwindow* window_ptr;

    struct Attributes {
        int width = 0;
        int height = 0;
        std::string const& title{};
        GLFWmonitor* monitor = nullptr;
        GLFWwindow* share = nullptr;
    };

private:
    Attributes attr;
    Context* ctx = nullptr;
    window_ptr pw = nullptr;
    std::function<void()> main_loop{};
    std::thread th{};

private:
    void process();
    void stop();

public:
    Window(Context& ctx, Attributes const& attr, std::function<void()> const& loop);

    void run();
    void swap_buffers();
    void set_default_viewport();
    void set_as_main_context();
    void poll_events();
    void set_close(int flag = 1);
    [[nodiscard]] bool should_close();
    [[nodiscard]] window_ptr native_handle();
};

class Context {
    friend class Window;

private:
    std::vector<Window*> windows;
    std::mutex m;
    std::queue<std::function<void()>> q;
    std::condition_variable poll_cv;

private:
    void add_window(Window* wd);
    void poll_events();

public:
    Context();
    ~Context();

    void exec();
    void submit(std::function<void()> const& f);
};
}

#endif //DGRAPH_CONTEXT_H
