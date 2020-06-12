#ifndef DGRAPH_CONTEXT_H
#define DGRAPH_CONTEXT_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <string>
#include <functional>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <mutex>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "dgl/error.h"

#include "util/cb_signal.h"
#include "util/cargs.h"
#include "util/timer.h"

namespace dgl {
class Context;

class Window {
    friend class Context;

public:
    typedef GLFWwindow  window_t;
    typedef GLFWwindow* window_ptr;
    typedef std::function<void(int, int, int, int)> key_cb_t;
    typedef std::function<void(double, double)> cur_cb_t;
    typedef cb_signal<int, int, int, int> key_sig_t;
    typedef cb_signal<double, double> cur_sig_t;

    struct Attributes {
        int width = 0;
        int height = 0;
        std::string title{};
        GLFWmonitor* monitor = nullptr;
        GLFWwindow* share = nullptr;
    };

protected:
    Attributes attr;
    Context* ctx = nullptr;
    window_ptr pw = nullptr;

private:
    std::thread th{};
    bool keys[512]{false};
    key_sig_t key_sig;
    cur_sig_t cur_sig;

protected:
    virtual void process() = 0;

private:
    void stop();

public:
    Window(Attributes const& attr);

    Window& operator=(Window&& w);

    Window(Window&& w);

    Window& operator=(Window const& w) = delete;

    Window(Window const& w) = delete;

    void swap(Window& w);

    void run();

    void swap_buffers();

    void set_default_viewport();

    void set_as_main_context();

    void poll_events();

    void set_close(int flag = 1);

    void key_callback(int key, int scancode, int action, int mode);

    void cur_callback(double xpos, double ypos);

    key_sig_t::connection reg_key_cb(key_cb_t const& cb);

    cur_sig_t::connection reg_cur_cb(cur_cb_t const& cb);

    bool pressed(int key) const noexcept;

    [[nodiscard]] bool should_close();

    [[nodiscard]] window_ptr native_handle();
};

class Context {
    friend class Window;

private:
    std::unordered_map<Window::window_ptr, Window*> windows;
    std::mutex m;
    std::queue<std::function<void()>> q;
    std::condition_variable poll_cv;
    timer tm;
    tmnl::cargs args_;

private:
    void add_window(Window* wd);

    void poll_events();

    int call_and_timeout() noexcept;

public:
    Context(int argc, char** argv);

    ~Context();

    Context& operator=(Context const&) = delete;

    Context(Context const&) = delete;

    void exec();

    void submit(std::function<void()> const& f);

    timer& get_timer() noexcept;

    tmnl::cargs const& args() const noexcept;

    Window* get_window(Window::window_ptr p);
};

Context* get_current_context() noexcept;
}


#endif //DGRAPH_CONTEXT_H
